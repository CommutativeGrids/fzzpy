#include "fzz.h"

#include <boost/functional/hash.hpp>

// phat headers
// wrapper algorithm that computes the persistence pairs of a given boundary matrix using a specified algorithm
#include <phat/compute_persistence_pairs.h>

// main data structure (choice affects performance)
#include <phat/representations/vector_vector.h>
#include <phat/representations/bit_tree_pivot_column.h>

// algorithm (choice affects performance)
#include <phat/algorithms/chunk_reduction.h>
#include <phat/algorithms/row_reduction.h>
#include <phat/algorithms/spectral_sequence_reduction.h>
#include <phat/algorithms/standard_reduction.h>
#include <phat/algorithms/twist_reduction.h>
#include <chrono>
#include <iostream>
#include <string>


namespace FZZ { 

template <class ElemType>
class VecHash { 
public:
    size_t operator()(const std::vector<ElemType>& v) const; 
};

template <class ElemType>
size_t VecHash<ElemType>
    ::operator()(const std::vector<ElemType>& v) const {

    std::size_t seed = 0;

    for (auto e : v) { boost::hash_combine(seed, e); }

    return seed;
}

template <class ElemType>
class VecEqual { 
public:
    bool operator()(const std::vector<ElemType>& v1, 
        const std::vector<ElemType>& v2) const; 
};

template <class ElemType>
bool VecEqual<ElemType>
    ::operator()(const std::vector<ElemType>& v1, 
        const std::vector<ElemType>& v2) const {

    if (v1.size() != v2.size()) { return false; }

    for (auto i = 0; i < v1.size(); i ++) {
        if (v1[i] != v2[i]) {
            return false;
        }
    }

    return true;
}

typedef std::unordered_map< Simplex, Integer,
    VecHash<Integer>, VecEqual<Integer> > SimplexIdMap;

void getBoundaryChainPhat(const std::vector<SimplexIdMap> &id_maps, 
    const Simplex &simp, std::vector<phat::index> &bound_c) {

    bound_c.clear();

    if (simp.size() <= 1) { return; }

    bound_c.reserve(simp.size());

    Simplex bound_simp(simp.begin()+1, simp.end());
    bound_c.push_back(id_maps.at(bound_simp.size() - 1).at(bound_simp));

    for (Integer i = 0; i < simp.size()-1; ++i) {
        bound_simp[i] = simp[i];
        bound_c.push_back(id_maps.at(bound_simp.size() - 1).at(bound_simp));
    }

    std::sort(bound_c.begin(), bound_c.end());
}

inline Integer getDim(const std::vector<phat::index> &bound_c) {
    if (bound_c.size() == 0) { return 0; }
    return bound_c.size() - 1;
}

class SimpleTimer {
public:
    SimpleTimer() : mStart(std::chrono::high_resolution_clock::now()) {}

    void report(const std::string& message) {
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - mStart).count();
        std::cout << message << " took: " << duration << "ms" << std::endl;
    }

private:
    std::chrono::high_resolution_clock::time_point mStart;
};


void FastZigzag::compute(const std::vector<Simplex> &filt_simp, 
        const std::vector<bool> &filt_op,
        std::vector< std::tuple<Integer, Integer, Integer> > *persistence,
        const std::string& algorithm) {

    std::cout << "Received " << filt_simp.size() << " simplices" << std::endl;

    // SimpleTimer timer;
    orig_f_add_id.clear();
    orig_f_del_id.clear();
    persistence->clear();

    simp_num = 0;
    Integer max_dim = 0;
    for (auto i = 0; i < filt_op.size(); ++i) {
        if (filt_op[i]) { 
            ++simp_num; 
            if (filt_simp[i].size() - 1 > max_dim) { max_dim = filt_simp[i].size() - 1; }
        }
    }
    // timer.report("Finished counting simplices and max dimension.");

    std::vector<phat::index> bound_c;
    // phat::boundary_matrix< phat::vector_vector > bound_chains;
    phat::boundary_matrix< phat::bit_tree_pivot_column > bound_chains;
    bound_chains.set_num_cols(simp_num * 2 + 1);

    // Add the Omega vertex for the coning
    bound_chains.set_col(0, bound_c);
    bound_chains.set_dim(0, 0);

    orig_f_add_id.reserve(simp_num);
    orig_f_del_id.reserve(simp_num);

    std::vector<Integer> del_ids;
    del_ids.reserve(simp_num);

    std::vector<SimplexIdMap> *p_id_maps = new std::vector<SimplexIdMap>(max_dim+1);
    std::vector<SimplexIdMap> &id_maps = *p_id_maps;

    Integer orig_f_id = 0;
    Integer s_id = 1;

    for (auto i = 0; i < filt_simp.size(); ++i) {
        const Simplex &simp = filt_simp[i];

        if (filt_op[i]) {
            getBoundaryChainPhat(id_maps, simp, bound_c);
            bound_chains.set_col(s_id, bound_c);
            bound_chains.set_dim(s_id, getDim(bound_c));

            // assert(s_id == bound_chains.size()-1);
            id_maps.at(simp.size() - 1)[simp] = s_id;
            orig_f_add_id.push_back(orig_f_id);
            s_id ++;
        } else {
            del_ids.push_back(id_maps.at(simp.size() - 1)[simp]);
            id_maps.at(simp.size() - 1).erase(simp);
            orig_f_del_id.push_back(orig_f_id);
        }

        orig_f_id ++;
    }

    for (Integer i = id_maps.size() - 1; i >= 0; -- i) {
        for (const auto &it : id_maps.at(i)) { 
            del_ids.push_back(it.second); 
            orig_f_del_id.push_back(orig_f_id);
            orig_f_id ++;
        }
    }
    // timer.report("Finished computing boundary matrix.");

    assert(del_ids.size() == s_id-1);
    delete p_id_maps;

    assert(simp_num == del_ids.size());

    std::vector<Integer> cone_sid(simp_num+1);

    for (auto del_id_it = del_ids.rbegin(); del_id_it != del_ids.rend(); ++del_id_it) {
        bound_c.clear();
        bound_c.push_back(*del_id_it);

        std::vector<phat::index> orig_bound_c;
        bound_chains.get_col(*del_id_it, orig_bound_c);

        if (orig_bound_c.size() == 0) {
            bound_c.push_back(0);
        } else {
            for (auto bsimp : orig_bound_c) {
                // assert(cone_sid[bsimp] >= 0);
                bound_c.push_back(cone_sid[bsimp]);
            }
        }

        std::sort(bound_c.begin(), bound_c.end());

        bound_chains.set_col(s_id, bound_c);
        bound_chains.set_dim(s_id, getDim(bound_c));

        cone_sid[*del_id_it] = s_id;

        s_id ++;
    }
    // timer.report("Finished coning.");

    phat::persistence_pairs pairs;

    if (algorithm == "chunk_reduction") {
        phat::compute_persistence_pairs< phat::chunk_reduction >( pairs, bound_chains );
    } else if (algorithm == "row_reduction") {
        phat::compute_persistence_pairs< phat::row_reduction >( pairs, bound_chains );
    } else if (algorithm == "spectral_sequence_reduction") {
        phat::compute_persistence_pairs< phat::spectral_sequence_reduction >( pairs, bound_chains );
    } else if (algorithm == "standard_reduction") {
        phat::compute_persistence_pairs< phat::standard_reduction >( pairs, bound_chains );
    } else if (algorithm == "twist_reduction") {
        phat::compute_persistence_pairs< phat::twist_reduction >( pairs, bound_chains );
    } else {
        throw std::invalid_argument("Invalid algorithm selection");
    }

    // // the most time-consuming line
    // phat::compute_persistence_pairs< phat::twist_reduction >( pairs, bound_chains );
    // phat::compute_persistence_pairs< phat::standard_reduction >( pairs, bound_chains );
    // phat::compute_persistence_pairs< phat::spectral_sequence_reduction >( pairs, bound_chains );
    // phat::compute_persistence_pairs< phat::chunk_reduction >( pairs, bound_chains );
    // // 
    // // For twist_reduction
    // auto start = std::chrono::high_resolution_clock::now();
    // phat::compute_persistence_pairs< phat::twist_reduction >( pairs, bound_chains );
    // auto end = std::chrono::high_resolution_clock::now();
    // auto duration_twist = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    // std::cout << "Time taken by twist_reduction: " << duration_twist << " ms" << std::endl;

    // // For spectral_sequence_reduction
    // start = std::chrono::high_resolution_clock::now();
    // phat::compute_persistence_pairs< phat::spectral_sequence_reduction >( pairs, bound_chains );
    // end = std::chrono::high_resolution_clock::now();
    // auto duration_spectral = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    // std::cout << "Time taken by spectral_sequence_reduction: " << duration_spectral << " ms" << std::endl;

    // // For chunk_reduction
    // start = std::chrono::high_resolution_clock::now();
    // phat::compute_persistence_pairs< phat::chunk_reduction >( pairs, bound_chains );
    // end = std::chrono::high_resolution_clock::now();
    // auto duration_chunk = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    // std::cout << "Time taken by chunk_reduction: " << duration_chunk << " ms" << std::endl;

    size_t total_pairs = pairs.get_num_pairs();
    // size_t progress_interval = total_pairs / 20; // This will print progress every 5%. Adjust as needed.
    // std::cout << "Total pairs: " << total_pairs << std::endl;
    for (phat::index idx = 0; idx < total_pairs; idx++) {
        Integer b = pairs.get_pair(idx).first;
        Integer d = pairs.get_pair(idx).second - 1;
        Integer p = bound_chains.get_dim(b);

        if (d < simp_num) { mapOrdIntv(b, d); } 
        else { mapRelExtIntv(p, b, d); }

        if (b > filt_simp.size()) { continue; }
        if (d > filt_simp.size()) { d = filt_simp.size(); }
        persistence->emplace_back(b, d, p);
        
        // // Print progression
        // if (idx % progress_interval == 0) {
        //     double progress = (double)idx / total_pairs * 100.0;
        //     std::cout << "\rProgress: " << std::fixed << std::setprecision(2) << progress << "%" << std::flush;
        // }
    }
    
    // std::cout << "Processed barcode has " << persistence->size() << " entries." << std::endl;
}

} 