/**
 * Copyright (C) 2019 Dean De Leo, email: hello[at]whatsthecraic.net
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <cstdint>
#include <memory>
#include <random>
#include <string>
#include <unordered_map>

#include "abtree.hpp"
#include "counting_tree.hpp"
#include "edge.hpp"

class Writer; // forward decl.

struct hotspot_edge_struct{
    hotspot_edge_struct(uint64_t s, uint64_t t, double w):source(s),destination(t),weight(w){}
    hotspot_edge_struct(const hotspot_edge_struct& other):source(other.source),destination(other.destination),weight(other.weight){}
    uint64_t source;
    uint64_t destination;
    double weight;
};
class Generator {

    Writer& m_writer; // serialise the operations in the log file

    uint64_t m_num_operations; // total number of operations (insertions/deletions of edges) to create
    uint64_t m_num_max_edges; // max number of edges that can be stored in the graph
    const uint64_t m_seed; // random generator seed

    uint64_t* m_vertices = nullptr; // vertices ID
    uint64_t m_num_vertices_final = 0; // num vertices that actually belong to the final graph
    uint64_t m_num_vertices_temporary = 0; // num vertices that are temporary, it will need to be removed from the final graphs
    static constexpr uint64_t m_num_final_edges_per_block = (1ull << 23); // number of `final' edges per block, 8M
    WeightedEdge** m_edges_final = nullptr; // list of vertices that belong to the final graph
    uint64_t m_num_edges_final = 0; // total number of edges
    CountingTree* m_frequencies = nullptr; // the frequency  associated to each vertex in the graph. Initially the frequency is the number of edges attached in the loaded graph.
    std::unordered_map<Edge, bool> m_edges_present; // edges present during the creation of the graph
    std::mt19937_64 m_random;

    void init_read_input_graph(void* ptr_edges_final, void* ptr_frequencies, const std::string& path_input_graph, double ef_vertices);
    void init_temporary_vertices(void* ptr_map_frequencies, void* ptr_array_frequencies, double sf_frequency);
    void init_counting_tree(void* ptr_array_frequencies);
    void init_permute_edges_final(std::unique_ptr<WeightedEdge[]>& ptr_edges_final);
    void init_writer(const std::string& path_log_file);

    // total number of blocks in the final edges
    uint64_t num_blocks_in_final_edges() const;

    // Actual generator, return the number of operations performed
    uint64_t generate0();


    uint64_t generate_hotspot_workload0();

    uint64_t generate_hotspot_workload1();

    uint64_t generate_hotspot_workload2();

    uint64_t generate_hotspot_workload3();

    void record_clustered_operation(std::unordered_map<uint64_t, std::vector<hotspot_edge_struct>>& clustered_by_source, uint64_t source, uint64_t destination, double weight);
public:
    // Constructor
    Generator(const std::string& path_input_graph, const std::string& path_output_log, Writer& writer, double sf_frequencies, double ef_vertices, double ef_edges, double aging_factor, uint64_t seed);

    // Destructor
    ~Generator();

    // Generate the operations to perform
    void generate();

    void generate_hotspot0();
    // Total number of vertices that will appear in the final graph
    uint64_t num_final_vertices() const { return m_num_vertices_final; }

    // Total number of temporary vertices
    uint64_t num_temporary_vertices() const { return m_num_vertices_temporary; }

    // Total number of (final) edges in the graph
    uint64_t num_edges() const { return m_num_edges_final; }

    // Total number of vertices generated
    uint64_t num_vertices() const { return num_final_vertices() + num_temporary_vertices(); }
};