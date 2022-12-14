//
//  graph_util.cpp
//  networkz
//
//  Created by CAO Wei on 2020/08/21.
//  Copyright © 2020 曹巍. All rights reserved.
//
#include <limits>  // std::numeric_limits<T>::max()
#include <boost/timer/timer.hpp>

#include "graph_util.hpp"

// -----------------------------------------------------------------------------

void mat2file(const Eigen::MatrixXd& mat, const std::string& filename)
{
  const static Eigen::IOFormat CSVFormat(Eigen::FullPrecision,
                                         Eigen::DontAlignCols,
                                         ", ",
                                         "\n");
  std::ofstream file(filename);
  if (file.is_open()) {
    file << mat.format(CSVFormat);
    file.close();
  }
}
// -----------------------------------------------------------------------------
//
//
Eigen::MatrixXd NARO::CityBlock::operator()(const Eigen::MatrixXd& mat,
                                            bool verbose=false) {
  int nrows = static_cast<int>(mat.rows());
  Eigen::MatrixXd d_mat(nrows, nrows);
  for (int rowIdx=0; rowIdx < nrows; rowIdx++) {
    d_mat.row(rowIdx) = (mat.rowwise() - mat.row(rowIdx)).matrix().rowwise().sum();
  }
//  std::cout << "Original matrix:\n";
//  std::cout << mat << std::endl;
//  std::cout << "City block distance matrix:\n";
//  std::cout << d_mat.cwiseAbs() << std::endl;
  if (verbose) {
    mat2file(d_mat.cwiseAbs(), "networkz_cityblock_mat.csv");
  }
  return d_mat.cwiseAbs();
}

// -----------------------------------------------------------------------------
// Eigen uses Column Major definition of Matrix in general if not specified.
//
Eigen::MatrixXd NARO::Euclidean::operator()(const Eigen::MatrixXd& mat,
                                            bool verbose=false)
{
  auto d_mat = ((-2 * mat.transpose() * mat).colwise() +
                mat.colwise().squaredNorm().transpose()).rowwise() +
               mat.colwise().squaredNorm();
  if (verbose) {
    mat2file(d_mat, "networkz_euclidean_mat.csv");
  }
  return d_mat;
}
// -----------------------------------------------------------------------------
// Function corrcoef
// Pearson's correlation coefficient for real numbers.
//   mat - the observations are arranged as rows.
//
Eigen::MatrixXd NARO::Corrcoef::operator()(const Eigen::MatrixXd& mat,
                                           bool verbose=false) {
  const double epsilon = 1e-9;
  // For measuring elapsed time
  std::chrono::duration<double> seconds;
  // manually start timer
  boost::timer::cpu_timer timer;
  timer.start();
  std::cout<< " Compute correlation matrix ...";
  double nrows = static_cast<double>(mat.rows());
  if (nrows == 1) {
    std::cerr << "Error: the input matrix at least contains two rows"
              << std::endl;
  }

  // Centered the data matrix
  auto m = mat.rowwise().mean();
  Eigen::MatrixXd Xc = mat.colwise() - m;
  // Calculate the covariance matrix
  Eigen::MatrixXd cov = Xc * Xc.transpose() / (nrows - 1);
  // Calculate the correlation coefficient
  // By matrix:
  //Eigen::VectorXd diag = cov.diagonal().array().sqrt();
  //Eigen::MatrixXd Dinv = diag.asDiagonal().inverse();
  //Eigen::MatrixXd cc = Dinv * cov * Dinv;
  // By array:
  Eigen::VectorXd diag = cov.diagonal();
  Eigen::VectorXd D = diag.array().rsqrt();
  Eigen::MatrixXd cc = ((cov.array().rowwise() * D.transpose().array())
                       .colwise() * D.array()).abs();
  Eigen::MatrixXd cc_dist = 1 - cc.array();
  
  // Correct values close to zero to be zero
  cc_dist = (cc_dist.array().abs() < epsilon).select(0., cc_dist);
  
  /*if (verbose) {
    std::cout<< "\nSave the intermediate files" << std::endl;
    //mat2file(cov, "networkz_covariance_mat.csv");
    //mat2file(cc, "networkz_corref_mat.csv");
    mat2file(cc_dist, "networkz_corref_dist_mat.csv");
  }
  */
  timer.stop();
  seconds = std::chrono::nanoseconds(timer.elapsed().user);
  std::cout << " Completed in " << seconds.count() << " seconds."
            << std::endl;
  return cc_dist;
}

// -----------------------------------------------------------------------------
//
bool create_graph_ct(NARO::Graph* g, NARO::DataFrame* df) {
  bool result = false;
  return result;
}
// -----------------------------------------------------------------------------
//
bool create_graph_corr(NARO::Graph* g, NARO::DataFrame* df)
{
  bool result = false;
  auto mat = df->data;
  // Centered the df
  Eigen::MatrixXd centered = mat.rowwise() - mat.colwise().mean();
  // Calculate pearson correlation coefficient
  Eigen::MatrixXd cov = (centered.adjoint() * centered) / double(mat.rows() - 1);
  // 
  return result;
}
// -----------------------------------------------------------------------------
// The observations arranged as row variables.
template<class DistType>
bool NARO::create_graph(NARO::Graph* g,
                        DataFrame* df,
                        double dist_threshold,
                        DistType dist_functor) {
  bool inserted;
  
  double total_weights = 0.0L;
  double max_weights = 0.0L;
  double min_weights = std::numeric_limits<double>::max();
  
  NARO::NameVertexMap name2vertex;
  NARO::NameVertexMap::iterator pos_u;
  NARO::NameVertexMap::iterator pos_v;
  NARO::Vertex u, v;
  
  // Get data:
  auto dist_mat = dist_functor(df->data, true);
  size_t num_rows = dist_mat.rows();
  auto rownames = df->get_rowIndex_names();
  if(rownames.size() != num_rows) {
    std::cerr <<
      "The input dataframe has errors: string index != number of rows"
    << std::endl;
  }
#pragma omp parallel for
  for(int i=0; i<(num_rows - 1); i++) {
    int n1_inx = i;
    std::string n1 = rownames[n1_inx];
    double d_n1 = (*df)(n1_inx, 0);
    // Insert one node
    boost::tie(pos_u, inserted) = name2vertex.insert(
                                      std::make_pair(n1, NARO::Vertex()));
    if (inserted) {
      u = boost::add_vertex(NARO::gVertex{n1, d_n1}, *g);
      pos_u->second = u;
    } else {
      u = pos_u->second;
    }

    for(int j=i+1; j<num_rows; j++) {
      int n2_inx = j;
      std::string n2 = rownames[n2_inx];
      double d_n2 = (*df)(n2_inx, 0);
      // Create the other node in the graph
      boost::tie(pos_v, inserted) = name2vertex.insert(
                                    std::make_pair(n2, NARO::Vertex()));
      // Create an edge between these two nodes
      // if the distance between them be less than the dist_threshold
      if (inserted) {
        v = boost::add_vertex(NARO::gVertex{n2, d_n2}, *g);
        //(*g)[boost::graph_bundle].name2v.insert(n2, v);
        pos_v->second = v;
      } else {
        v = pos_v->second;
      }
      double d = dist_mat(i, j);
      if (d < dist_threshold ) {
        // total sum of weights
        if (d < 0) {
          std::cerr << "ERROR:The weights of the edge is negative"<< std::endl;
          std::cerr << n1 << " -- " << n2 <<": "<< d << std::endl;
          std::exit(-1);
        }
        // Create an edge conecting those two vertices
        boost::add_edge(u, v, NARO::gEdge{d}, *g);
        // Update the max and min weights
        if (d > max_weights) {
          max_weights = d;
        }
        if (d < min_weights) {
          min_weights = d;
        }
        // Sum the weights
        total_weights += d;
      }
    }
  }
  
  // Add the graph properties
  (*g)[boost::graph_bundle].total_weights = total_weights;
  (*g)[boost::graph_bundle].max_weights = max_weights;
  (*g)[boost::graph_bundle].min_weights = min_weights;
  (*g)[boost::graph_bundle].num_vertices =
        static_cast<int>(boost::num_vertices(*g));
  (*g)[boost::graph_bundle].num_edges =
        static_cast<int>(boost::num_edges(*g));
 
  // Assign component id
  int n_vertices = static_cast<int>(boost::num_vertices(*g));
  std::vector<int> component(n_vertices);
  int num = boost::connected_components(*g, &component[0]);
  (*g)[boost::graph_bundle].num_component = num;
  int comm_id = 0;
  NARO::VertexIter vi, vend;
  for(boost::tie(vi, vend) = boost::vertices(*g); vi != vend; ++vi) {
    (*g)[*vi].communityId = comm_id;
    (*g)[*vi].componentId = component[*vi];
    comm_id++;
  }
  return true;
}
// -----------------------------------------------------------------------------
// Specialization of create_graph
// City block distance
// -----------------------------------------------------------------------------
template
bool NARO::create_graph<NARO::CityBlock>(NARO::Graph *g, DataFrame *df,
                                         double dist_threshold,
                                         NARO::CityBlock dist_functor);
// -----------------------------------------------------------------------------
// Specialization of create_graph
// Euclidean distance
// -----------------------------------------------------------------------------
template
bool NARO::create_graph<NARO::Euclidean>(NARO::Graph *g, DataFrame *df,
                                         double dist_threshold,
                                         NARO::Euclidean dist_functor);
// -----------------------------------------------------------------------------
// Specialization of create_graph
// Correlation coefficient
// -----------------------------------------------------------------------------
template
bool NARO::create_graph<NARO::Corrcoef>(NARO::Graph *g, DataFrame *df,
                                         double dist_threshold,
                                         NARO::Corrcoef dist_functor);

// -----------------------------------------------------------------------------
// Function
//
// -----------------------------------------------------------------------------
bool NARO::create_graph(NARO::Graph* g, DataFrame* df, double dist_threshold,
                        std::string distance_type)
{
  bool result = false;
  if (distance_type == "city") {
    NARO::CityBlock dist;
    result = NARO::create_graph(g, df,dist_threshold, dist);
  }else if (distance_type == "euc") {
    NARO::Euclidean dist;
    result = NARO::create_graph(g, df, dist_threshold, dist);
  } else if (distance_type == "corr"){
    NARO::Corrcoef dist;
    result = NARO::create_graph(g, df,dist_threshold, dist);
  } else {
    std::cerr << "Unknown method for distance computation" << std::endl;
  }
  return result;
}

// -----------------------------------------------------------------------------
//
bool NARO::write_to_graphviz(std::string& filename, NARO::Graph& g)
{
  // Write the graph to the output file
  NARO::fs::path path = filename;
  if (!NARO::fs::exists(path)){
    if (!path.parent_path().empty()) {
      try {
        // Create directory
        NARO::fs::create_directory(path.parent_path());
      }
      catch (NARO::fs::filesystem_error& err) {
        std::cerr << err.what() << std::endl;
      }
    }
  }
  std::ofstream graphfile(filename);
  boost::write_graphviz(graphfile, g,
    /* Vertex */
    [&] (auto& out, auto v) {
      out << g[v].to_graphviz();},
    /* Edge */
    [&] (auto& out, auto e) {
    out << g[e].to_graphviz();},
    /* Graph property */
    [&] (auto& out) {out<< g.m_property->to_graphviz();}
  );
  graphfile.close();
  return true;
}

// -----------------------------------------------------------------------------
// Write the components to a txt
//
bool NARO::write_components(const std::string& filename,
                            std::string& inputfile,
                            NARO::Graph* g,
                            const std::string& date)
{
  // Write the graph to the output file
  NARO::fs::path path = filename;
  if (!NARO::fs::exists(path)){
    if (!path.parent_path().empty()) {
      try {
        // Create directory
        NARO::fs::create_directory(path.parent_path());
      }
      catch (NARO::fs::filesystem_error& err) {
        std::cerr << err.what() << std::endl;
      }
    }
  }
  std::ofstream txtfile(filename);

  auto n_vertices = boost::num_vertices(*g);
  std::vector<int> component(n_vertices);
  int num = boost::connected_components(*g, &component[0]);

  std::vector< int >::size_type i;
  NARO::VertexIter vi, vend;
  txtfile << "# File: "<< inputfile << " \n";
  txtfile << "# Date: " << date << " \n";
  txtfile << "# Total components: " << num << " \n";
  for (i=0; i != component.size(); ++i) {
    for(boost::tie(vi, vend) = boost::vertices(*g); vi != vend; ++vi) {
      auto vertex_name = (*g)[*vi].name;
      auto degree = boost::degree(*vi, *g);
      auto component_num = component[*vi];
      if (component_num == i)
        txtfile << vertex_name << ":" << std::to_string(component_num) << ": "
                << std::to_string(degree) << "  \n";
    }
  }

  return true;
}

// -----------------------------------------------------------------------------
//
//
void NARO::convert(NARO::Graph& g,
                   NARO::Algo::Community::CSRgraph& csr_g,
                   //std::vector<int>* n2c,
                   //std::vector<std::string>* lookup_table)
                   //std::map<unsigned int, std::string>* n2str_table)
                   std::map<std::string, unsigned int>* str2node)
{
  int node_size = static_cast<int>(boost::num_vertices(g));
  csr_g.nb_nodes = node_size;
  csr_g.sum_nodes_w = node_size;
  csr_g.degrees.resize(node_size);
  csr_g.nodes_w.assign(node_size, 1);
  
  std::vector<std::vector<std::pair<int, long double>>> links;
  //std::map<std::string, unsigned int> str2node;

  // Add nodes' degrees
  NARO::VertexIter vi, vend;
  int node_id = 0;
  for(boost::tie(vi, vend) = boost::vertices(g); vi != vend; ++vi) {
    (*str2node)[g[(*vi)].name] = node_id;
    csr_g.degrees[node_id] = boost::degree(*vi, g);
    node_id++;
  }
  // cumulative degree sequence
  // cum_degree[0]=degree(0); cum_degree[1]=degree(0)+degree(1), etc.
  for (int i=1; i<csr_g.degrees.size(); i++) {
    csr_g.degrees[i] += csr_g.degrees[i-1];
  }
  // Read links: for each link (each link is counted twice)
  csr_g.nb_links = csr_g.degrees[csr_g.nb_nodes-1];
  
  // Iterate over the edges
  auto es = boost::edges(g);
  int nId_src = 0;
  int nId_dest = 0;

  for (auto eit = es.first; eit != es.second; ++eit) {
    auto weight = g[*eit].distance;
    NARO::Vertex src = boost::source(*eit, g);
    NARO::Vertex dest = boost::target(*eit, g);
    
    nId_src = (*str2node)[g[src].name];
    nId_dest = (*str2node)[g[dest].name];
    if (links.size() <= std::max(nId_src, nId_dest) + 1){
      links.resize(std::max(nId_src, nId_dest) + 1);
    }
    links[nId_src].push_back(std::make_pair(nId_dest, weight));
    if (src != dest) {
      links[nId_dest].push_back(std::make_pair(nId_src, weight));
    }
  }

  int s = static_cast<int>(links.size());
  for (int i=0; i<s; i++) {
    for (unsigned int j=0; j<links[i].size(); j++) {
      int dest = links[i][j].first;
      long double weight = links[i][j].second;
      csr_g.links.push_back(dest);
      csr_g.weights.push_back(weight);
    }
  }
  // Compute total weight
  for (int i=0 ; i<csr_g.nb_nodes ; i++)
    csr_g.total_weight += csr_g.weighted_degree(i);
}
