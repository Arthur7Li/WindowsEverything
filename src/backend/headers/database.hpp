#pragma once

#include "common.hpp"
#include "cover/cover.hpp"
#include "general.hpp"
#include "sqlite.hpp"

std::vector<PointQ> bounding_line_segment(const CodeSequence& code_sequence, const InitialAngles& initial_angles);



std::vector<PointQ> bounding_polygon(const CodeSequence& code_sequence, const InitialAngles& initial_angles);

// arthur 06/08/2026 [Updated signatures to use ConnectionPool for thread-safe concurrent database lookups]
/**
 * @brief Retrieves stable information for a set of code sequences.
 * @param code_seqs The set of code sequences to process.
 * @param mrr If true, uses the MRR schema, otherwise all schema.
 * @param pool The SQLite connection pool to borrow thread-safe connections from.
 * @return std::vector<std::pair<SinglePair, StableInfo>> Resulting stable info pairs, sorted by cost.
 * @note Ownership of the connections remains with the pool.
 * @warning Throws runtime_error on SQLite query failure.
 * @invariant Sequences returned match the requested set.
 * @math Resolves boundary geometries for sequence stability.
 */
std::vector<std::pair<SinglePair, StableInfo>> get_single_infos(const std::set<CodeSequence>& code_seqs, const bool mrr, sqlite::ConnectionPool& pool);

/**
 * @brief Retrieves stable and unstable information for a set of triples.
 * @param triples The set of triples to process.
 * @param mrr If true, uses the MRR schema, otherwise all schema.
 * @param pool The SQLite connection pool.
 * @return std::vector<std::pair<TriplePair, TripleInfo>> Resulting triple info pairs.
 * @note Ownership of the connections remains with the pool.
 * @warning Throws runtime_error on SQLite query failure.
 * @invariant Triples returned match the requested set.
 * @math Evaluates positive and negative stability constraints.
 */
std::vector<std::pair<TriplePair, TripleInfo>> get_triple_infos(const std::set<Triple>& triples, const bool mrr, sqlite::ConnectionPool& pool);

/**
 * @brief Evaluates triple duplicate stability with early exit.
 * @param triples The set of triples.
 * @param mrr True for MRR.
 * @param pool Connection pool.
 * @param show True to print debug output.
 * @return std::pair<bool, bool> Indicates if stable and positive.
 * @note Used for duplicate stability tests.
 * @warning Throws runtime_error on SQLite query failure.
 * @invariant Evaluates the geometric cover properties of the triples.
 * @math Checks corner overlap for duplicate stable boundaries.
 */
std::pair<bool, bool> get_triple_infos_duplicate_stables(const std::set<Triple>& triples, const bool mrr, sqlite::ConnectionPool& pool, const bool show);

/**
 * @brief Evaluates half triple duplicate stability.
 * @param half_triples The set of half triples.
 * @param mrr True for MRR.
 * @param pool Connection pool.
 * @return bool True if positive cover found.
 * @note Used for duplicate stability tests.
 * @warning Throws runtime_error on SQLite query failure.
 * @invariant Evaluates the geometric cover properties of the half triples.
 * @math Checks overlap for single stability boundaries.
 */
bool get_triple_infos_half_duplicate_stables(const std::set<HalfTriple>& half_triples, const bool mrr, sqlite::ConnectionPool& pool);
std::pair<InitialAngles, CodeInfo> get_stable_info(const CodeSequence& code_sequence, const bool mrr, sqlite::Database& db);

/**
 * @brief Maps CodePairs to their stable info.
 * @param code_seqs Vector of CodePairs.
 * @param mrr True for MRR.
 * @param pool Connection pool.
 * @return std::map<SinglePair, StableInfo> Result map.
 * @note Validates angles match the cached data.
 * @warning Throws runtime_error if stable angles do not match.
 * @invariant The map contains exactly the requested CodePairs.
 * @math Loads complex periodic vectors.
 */
std::map<SinglePair, StableInfo> get_single_infos_map(const std::vector<CodePair>& code_seqs, const bool mrr, sqlite::ConnectionPool& pool);

/**
 * @brief Maps TriplePairs to their triple info.
 * @param triples Vector of TriplePairs.
 * @param mrr True for MRR.
 * @param pool Connection pool.
 * @return std::map<TriplePair, TripleInfo> Result map.
 * @note Validates triple pairs match the cached data.
 * @warning Throws runtime_error if triple pairs do not match.
 * @invariant The map contains exactly the requested TriplePairs.
 * @math Loads triple geometry intersections.
 */
std::map<TriplePair, TripleInfo> get_triple_infos_map(const std::vector<TriplePair>& triples, const bool mrr, sqlite::ConnectionPool& pool);

CodeInfo calculate_stable_all_info(const CodeSequence& code_sequence, const InitialAngles& initial_angles);
CodeInfo calculate_unstable_all_info(const CodeSequence& code_sequence, const InitialAngles& initial_angles);
CodeInfo calculate_all_info(const CodeSequence& code_sequence, const InitialAngles& initial_angles);
CodeInfo calculate_all_vector(const CodeSequence& code_sequence, const InitialAngles& initial_angles);
