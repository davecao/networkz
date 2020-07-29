//
//  main.cpp
//  networkz
//
//  Created by 曹巍 on 2020/07/25.
//  Copyright © 2020 曹巍. All rights reserved.
//
//
/* -----------------------------------------------------------------------------
 * Compilation:
 * clang++ -Os -std=c++17 main.cpp -o networkz \
 *      -I/Users/davecao/dist/eigen/include/eigen3 \
 *      -I/Users/davecao/dist/boost/include \
 *      -L/Users/davecao/dist/boost/lib \
 *      -lboost_program_options -lboost_iostreams -lboost_timer -lboost_graph \
 *      -lc++
 *
 * Usage:
 *  General options:
 *  -V [ --version ]        Show the version number
 *  -i [ --infile ] arg     The input data file. Only the tsv format is supported
 *                          now.
 *  -o [ --outfile ] arg    The output file of clusters in text format.Default
 *                          name is 'o_graphviz.gv'.
 *  -h [ --help ]           print help info.
 *
 * Graph options:
 *  -v [ --verbose ]        The extra verbose.
 *  -t [ --threshold ] arg  The threshold between two vertices which are linked
 *                          by an edge if the distance less than it. default is
 *                          0.
 *
 * Example:
 *  1. Create a graphviz file.
 *  $> ./networkz -i /path/to/your/abundance.tsv -t 2.0 -o abundance.gv
 *  2. Generate a png by `dot` command in Graphviz package.
 *  $> dot -Tpng o_graphviz.gv -o o_graphviz.png
 * ---------------------------------------------------------------------------*/

#include <Eigen/Core>

#include "common.hpp"
#include "cli_opt.hpp"
//#include "print.hpp"

struct gVertex {
  std::string name;
  double tpm;
  
  std::string to_graphviz() {
    std::stringstream ss;
    ss << "[label=\"" << name << "\", tpm=" << tpm << "]";
    return ss.str();
  }
};

struct gEdge {
  double distance;
  
  std::string to_graphviz() {
    std::stringstream ss;
    ss << "[weight=" << distance << ", "
    << "arrowhead=\"none\", color=\"purple\"]";
    return ss.str();
  }
};

struct gGraph {
  std::string name;
  
  std::string to_graphviz() {
    std::stringstream ss;
    ss << "label=\""<<name << ";\n"
       << "node [shape=\"circle\", filled=\"none\", color=\"purple\"];\n"
       << "edge [arrowType=\"dot\", color=\"purple\"];\n";
    return ss.str();

  }
};

typedef Eigen::MatrixXd Dynamic2D;
//typedef std::unordered_map<TupleKey, double, KeyHash, KeyEqual> EdgesMap;

typedef boost::adjacency_list<
  boost::listS, // edge container type: list
  boost::vecS,  // vertex container type: list
  boost::undirectedS, // Graph type: undirected graph
  gVertex, // bundled property for vertex
  gEdge,   // bundled property for edge
  gGraph   // bundled property for graph
> Graph;
// Graph-specific definitions
typedef boost::graph_traits<Graph>::vertex_descriptor Vertex;
typedef boost::graph_traits<Graph>::edge_descriptor Edge;
typedef boost::vertex_bundle_type<Graph>::type VertexType;
// Container for testing the existence of a vertex in the graph
typedef std::map<std::string, Vertex> NameVertexMap;

/**
 * Show Eigen version information.
 *
 */
void show_eigen_info() {
  std::cout << "Eigen version: "
            << EIGEN_MAJOR_VERSION << "."
            << EIGEN_MINOR_VERSION
  << std::endl;
}

/**
 *  Boost version information.
 *
 */
void show_boost_version() {
  std::cout << "Boost version: "
            << BOOST_VERSION / 100000     << "."  // major version
            << BOOST_VERSION / 100 % 1000 << "."  // minor version
            << BOOST_VERSION % 100                // patch level
  << std::endl;
}


struct Index
{
  std::map<std::string, size_t> Names;
  std::vector<std::pair<std::string, size_t>> OrderedNames;
  bool sorted = false;

  ssize_t GetIndex(std::string& name) const {
    auto it = Names.find(name);
    if ( it != Names.end())
    {
      return it->second;
    }
    return -1;
  }
  
  std::vector<std::string> GetIndexNames() {
    std::vector<std::string> keys;
    if (!this->sorted) {
      this->SortIndex();
      this->sorted = true;
    }
    for (const auto& [key, ignored] : OrderedNames) {
        keys.push_back(key);
    }
    return keys;
  }

private:

  void SortIndex(){
    // Construct a vector for sorting values
    std::vector<std::pair<std::string, size_t>> v{
      std::make_move_iterator(begin(this->Names)),
      std::make_move_iterator(end(this->Names))};
    // Sorting
    /*
    std::sort(std::execution::par, begin(v), end(v),
              [](auto p1, auto p2){return p1.second < p2.second;});
     */
    std::sort(begin(v), end(v),
              [](auto p1, auto p2){return p1.second < p2.second;});
    this->OrderedNames = v;
  }
};

struct DataFrame
{
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
  size_t num_rows = 0;
  size_t num_cols = 0;

  Index *rowIndex;
  Index *columnIndex;
  
  Dynamic2D data;

  DataFrame() {
    this->rowIndex = new Index();
    this->columnIndex = new Index();
  }
  
  DataFrame(size_t nrows, size_t ncols) {
    this->num_rows = nrows;
    this->num_cols = ncols;
    this->data.resize(nrows, ncols);
    this->rowIndex = new Index;
    this->columnIndex = new Index;
  }
  
  ~DataFrame() {
    delete rowIndex;
    delete columnIndex;
  }

  void resize(size_t nrows, size_t ncols) {
    this->num_rows = nrows;
    this->num_cols = ncols;
    this->data.resize(nrows, ncols);
  }

  size_t size() {
    return num_rows;
  }

  void set_data(Dynamic2D& d) {
    this->data = d;
  }
  
  double operator()(int r_inx, int c_inx) {
    return this->data(r_inx, c_inx);
  }

  bool set_columnIndex_names(std::string& col_name, size_t i){
    this->columnIndex->Names.insert(
        std::pair<std::string, size_t>(col_name, i));
    return true;
  }

  bool set_columnIndex_names(std::vector<std::string>& col_name){
    for(size_t i = 0; i != col_name.size(); i++){
      this->columnIndex->Names.insert(
        std::pair<std::string, size_t>(col_name[i], i));
    }
    return true;
  }
  
  std::vector<std::string> get_columnIndex_names() {
    return this->columnIndex->GetIndexNames();
  }
  
  bool set_rowIndex_names(std::vector<std::string>& row_name) {
    for(size_t i = 0; i != row_name.size(); i++){
      this->rowIndex->Names.insert(
        std::pair<std::string, size_t>(row_name[i], i));
    }
    return true;
  }

  std::vector<std::string> get_rowIndex_names() {
    return this->rowIndex->GetIndexNames();
  }
  
  bool add_row(std::vector<std::string>& row, size_t rInx){
    if ((row.size() - 1) != num_cols) {
      std::cerr << "Malformat: the number of columns does not match"
      << std::endl;
        return false; //std::exit(-1);
    }
    // Row index
    this->rowIndex->Names.insert(std::pair<std::string, size_t>(row[0], rInx));
    // Data
    for(size_t i = 1; i <= num_cols; i++){
      //std::cout << row[0] <<" at Line " << rInx << "." <<std::endl;
      this->data(rInx, i-1) = std::stod(row[i]);
    }
    return true;
  }
  /*
   * Select data by a column name and return a copy of sub-dataset.
   *  time-consuming and may cause the memory leaks
   */
  DataFrame select(std::string& c_name) {

    size_t inx = this->columnIndex->GetIndex(c_name);
    std::vector<std::string> rinx = this->rowIndex->GetIndexNames();
    if (inx == -1) {
      // Column name - Not found
      std::cout<< c_name << " Not found." << std::endl;
      std::exit(-1);
    }
    Dynamic2D sub = this->data(Eigen::all, inx-1);
    DataFrame *sliced = new DataFrame(sub.rows(), sub.cols());
    sliced->set_columnIndex_names(c_name, 0);
    sliced->set_rowIndex_names(rinx);
    sliced->set_data(sub);

    return *sliced;
  }
  /*
   * Modify the passed dataframe and store the sub dataset.
   * Release the memory from the main function to avoid the memory leaks.
   */
  bool select(std::string& c_name, DataFrame* sliced) {
    size_t inx = this->columnIndex->GetIndex(c_name);
    std::vector<std::string> rinx = this->rowIndex->GetIndexNames();
    if (inx == -1) {
      // Column name - Not found
      //std::cout<< c_name << " Not found." << std::endl;
      return false;
    }
    Dynamic2D sub = this->data(Eigen::all, inx-1);
    sliced->resize(sub.rows(), sub.cols());
    //DataFrame *sliced = new DataFrame(sub.rows(), sub.cols());
    sliced->set_columnIndex_names(c_name, 0);
    sliced->set_rowIndex_names(rinx);
    sliced->set_data(sub);

    return true;
  }
  
  Dynamic2D select(std::vector<std::string>& sel_columns) {
    Dynamic2D dat;
    std::vector<size_t> col_inx;
    for (int i=0; i<sel_columns.size(); i++) {
      size_t inx = this->columnIndex->GetIndex(sel_columns[i]);
      if (inx == -1) {
        // Column name - Not found
        std::cout<< sel_columns[i] << " Not found." << std::endl;
        std::exit(-1);
      }
      col_inx.push_back(inx-1);
    }
    // Return submatrix of data
    return this->data(Eigen::all, col_inx);
  }

};


bool read_tsv(const std::string& tsv_file,
              DataFrame* df,
              std::string sep="\t",
              std::string comment="#",
              int header=0) {
  
  std::vector<std::string> lines;
  // read the file and skip comment lines
  lines = readFileToLines(tsv_file, comment);

  std::vector<std::string> l;
  boost::split(l, lines[0], boost::is_any_of(sep));
  
  // Get the number of rows and columns
  size_t nrows = static_cast<size_t>(lines.size());
  size_t ncols = static_cast<size_t>(l.size());
  
  // Contain a header line or not?
  if (header < 0) {
    // No header line if header < 0
    df->resize(nrows, ncols);
  } else if (header == lines.size()){
    std::cerr << "No data found" << std::endl;
    std::exit(-1);
  } else if (header > lines.size()) {
    std::cerr << "Wrong input argument: header="<< header << std::endl;
    std::cerr << "The argument value is larger than available lines."
              << std::endl;
    std::exit(-1);
  } else {
    df->resize(nrows - 1, ncols - 1);
    // Set the header line
    boost::split(l, lines[header], boost::is_any_of(sep));
    if (!df->set_columnIndex_names(l)){
      std::cerr << "Failed to set the header line." << std::endl;
    }
  }

  // Read data
  size_t row_counter = 0;
  for(size_t i = header + 1; i < lines.size(); i++){
    // split the line by the separator, boost::split
    boost::split(l, lines[i], boost::is_any_of(sep));

    // Store record lines
    if (!df->add_row(l, row_counter)) {
      std::cerr << "Failed to parse the format at line " << i << std::endl;
    }
    // increase the index
    ++row_counter;
  }
  return true;
}

int main(int argc, const char * argv[]) {
  
  namespace fs = std::filesystem;
  std::chrono::duration<double> seconds;

  // Parse cmd line arguments
  CLIARG::ParseCmdLine(argc, argv);
  std::string filename = CLIARG::i_filename;
  std::string graphviz_file = CLIARG::o_filename;
  
  std::string sep = "\t";
  std::string comment = "#";
  int header = 0;
  
  DataFrame *df = new DataFrame();
  // Show backend dependency
  show_eigen_info();
  show_boost_version();

  // manually start timer
  boost::timer::cpu_timer timer;
  timer.start();
  // 1. Read the tsv:
  if (read_tsv(filename, df, sep, comment, header)) {
    timer.stop();
    auto err = std::error_code{};
    auto filesize = byteConverter_s(fs::file_size(filename, err));
    seconds =
      std::chrono::nanoseconds(timer.elapsed().user);
    std::cout << "Read "<< filename << "(" << filesize  << ") completed."
    << " in " << seconds.count() << " seconds." << std::endl;
  }
  // 1.1 extract data ->  target_id  tpm
  std::cout << "Extract data ...";
  timer.start();
  std::string col = "tpm";
  auto rownames = df->get_rowIndex_names();
  //auto dat = df->select(col);
  DataFrame *dat = new DataFrame();
  if (!df->select(col, dat)) {
    std::cout<< col << " Not found." << std::endl;
    std::exit(-1);
  }
  std::vector<int> sel_inx;
  for (int i=0; i<dat->size(); i++) {
    if ((*dat)(i, 0) != 0) {
      sel_inx.push_back(i);
    }
  }
  timer.stop();
  seconds = std::chrono::nanoseconds(timer.elapsed().user);
  std::cout << " Completed in " << seconds.count() << " seconds." << std::endl;
  // 2. Compute the cityblock distance between any two genes
  //Instantiate a graph
  std::cout << "Create a graph ... ";
  timer.start();
  Graph genes_graph(gGraph{"Gene Expression Network"});
  
  NameVertexMap name2vertex;
  NameVertexMap::iterator pos_u;
  NameVertexMap::iterator pos_v;
  bool inserted;
  Vertex u, v;

  for(int i = 0; i < sel_inx.size() - 1; i++) {
    int n1_inx = sel_inx[i];
    std::string n1 = rownames[n1_inx];
    double d_n1 = (*dat)(n1_inx, 0);
    boost::tie(pos_u, inserted) = name2vertex.insert(
                                      std::make_pair(n1, Vertex()));
    if (inserted) {
      u = boost::add_vertex(gVertex{n1, d_n1}, genes_graph);
      pos_u->second = u;
    } else {
      u = pos_u->second;
    }

    for (int j = i + 1; j < sel_inx.size(); j++) {
      int n2_inx = sel_inx[j];
      std::string n2 = rownames[n2_inx];
      double d_n2 = (*dat)(n2_inx, 0);
      // Create two vertices in the graph
      boost::tie(pos_v, inserted) = name2vertex.insert(
                                    std::make_pair(n2, Vertex()));
      if (inserted) {
        v = boost::add_vertex(gVertex{n2, d_n2}, genes_graph);
        pos_v->second = v;
      } else {
        v = pos_v->second;
      }
      double d = abs(d_n1 - d_n2);
      if (d < CLIARG::d_threshold ) {
        // Create an edge conecting those two vertices
        boost::add_edge(u, v, gEdge{d}, genes_graph);
      }
    }
  }
  timer.stop();
  seconds = std::chrono::nanoseconds(timer.elapsed().user);
  std::cout << " Completed in " << seconds.count() << " seconds." << std::endl;
  
  std::cout << "Write to a " << graphviz_file;
  timer.start();
  // Write the graph to the output file
  std::ofstream graphfile(graphviz_file);
  /*
  boost::write_graphviz(std::cout, genes_graph,
    [&] (auto& out, auto v) {
      out << "[label=\"" << genes_graph[v].name << "\"]";
    },
    [&] (auto& out, auto e) {
      out << "[label=\"" << genes_graph[e].distance << "\"]";
  });
  std::cout << std::flush;
  */
  boost::write_graphviz(graphfile, genes_graph,
    /* Vertex */
    [&] (auto& out, auto v) {
      out << genes_graph[v].to_graphviz();},
    /* Edge */
    [&] (auto& out, auto e) {
    out << genes_graph[e].to_graphviz();},
    /* Graph property */
    [&] (auto& out) {out<< genes_graph.m_property->to_graphviz();}
  );
  graphfile.close();

  timer.stop();
  seconds = std::chrono::nanoseconds(timer.elapsed().user);
  std::cout << " completed in " << seconds.count() << " seconds." << std::endl;

  timer.start();
  // Release Memory
  delete df;
  delete dat;
  timer.stop();
  seconds = std::chrono::nanoseconds(timer.elapsed().user);
  std::cout << "Clean memory "
            << "in " << seconds.count() << " seconds." << std::endl;
  std::cout << "Program Exit." << std::endl;
  return EXIT_SUCCESS;
}
