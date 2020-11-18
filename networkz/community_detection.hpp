//
//  community_detection.hpp
//  networkz
//
//  Created by CAO Wei on 2020/11/18.
//  Copyright © 2020 曹巍. All rights reserved.
//

#ifndef community_detection_hpp
#define community_detection_hpp

#include <iostream>
#include <iomanip>

#include "graph.hpp"

namespace NARO::Algo::modularity {
/**
 * @brief Calculate Newman's (generalized) modularity of a connected network
 *
 * @param[in] g A undirected graph.
 * @param[in] Q Newman's modularity.
 *
 * Reference:
 *  M.E.J. Newman, "Modularity and community structure in networks",
 *  Proc. Natl. Acad. Sci. USA 103, 8577-8582(2006)
 */
float newman_comm(NARO::Graph& g, float gamma);

/**
 * @brief Calculate the quality for a given graph (connected components)
 *
 * @param[in] g A weighted undirected graph
 * @param[out] quality quality of the graph ``g``
 */
float quality(NARO::Graph& g);

/**
 * @brief Louvain's community detection  for a weighted undirected graph (connected components)
 *
 * @param[in] g A weighted undirected graph
 * @return none
 */
void louvain_comm(NARO::Graph& g);
}
#endif /* community_detection_hpp */
