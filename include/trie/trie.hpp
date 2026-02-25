/**
 * @file trie.hpp
 * @brief Minimal Trie (prefix tree) with autocomplete and ranked fuzzy search.
 *
 * Notes:
 * - Stores UTF-8 bytes as-is (char). For full Unicode grapheme support, preprocess input upstream.
 * - This library is intentionally small and deterministic.
 */

#ifndef TRIE_TRIE_HPP
#define TRIE_TRIE_HPP

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace trie
{
  /**
   * @brief Trie node.
   */
  struct TrieNode final
  {
    std::unordered_map<char, std::unique_ptr<TrieNode>> children{};
    bool is_terminal{false};
    std::uint32_t frequency{0};
  };

  /**
   * @brief Autocomplete trie with optional ranked fuzzy search.
   *
   * Features:
   * - insert(word)
   * - contains(word)
   * - suggest(prefix, limit)
   * - search_ranked(query, limit)
   *
   * Thread safety:
   * - By default, this class is NOT thread-safe.
   * - If you need concurrent operations, enable locking by passing thread_safe=true to the constructor.
   */
  class Trie final
  {
  public:
    /**
     * @brief Construct an empty trie.
     * @param thread_safe If true, operations lock an internal mutex.
     */
    explicit Trie(bool thread_safe = false)
        : root_(std::make_unique<TrieNode>()),
          thread_safe_(thread_safe)
    {
    }

    /**
     * @brief Insert a word into the trie.
     * @param word The word to insert.
     */
    void insert(const std::string &word)
    {
      LockGuard lock(*this);

      TrieNode *node = root_.get();
      for (char c : word)
      {
        auto &next = node->children[c];
        if (!next)
        {
          next = std::make_unique<TrieNode>();
        }
        node = next.get();
      }

      node->is_terminal = true;
      node->frequency += 1;
    }

    /**
     * @brief Check if a word exists in the trie.
     * @param word The word to look up.
     * @return true if present, false otherwise.
     */
    bool contains(const std::string &word) const
    {
      LockGuard lock(*this);

      const TrieNode *node = root_.get();
      for (char c : word)
      {
        const auto it = node->children.find(c);
        if (it == node->children.end())
        {
          return false;
        }
        node = it->second.get();
      }
      return node->is_terminal;
    }

    /**
     * @brief Return prefix suggestions.
     * @param prefix Prefix to complete.
     * @param limit Max number of results. If 0, returns all matches.
     */
    std::vector<std::string> suggest(const std::string &prefix, std::size_t limit = 0) const
    {
      LockGuard lock(*this);

      const TrieNode *node = root_.get();
      for (char c : prefix)
      {
        const auto it = node->children.find(c);
        if (it == node->children.end())
        {
          return {};
        }
        node = it->second.get();
      }

      std::vector<std::string> out;
      std::string current = prefix;
      collect_suggestions(node, current, out, limit);
      return out;
    }

    /**
     * @brief Ranked fuzzy search.
     *
     * Scans all terminal words in the trie and ranks them against @p query.
     * This is intentionally simple and deterministic.
     *
     * @param query Query string.
     * @param limit Max number of results. If 0, returns all matches.
     */
    std::vector<std::string> search_ranked(const std::string &query, std::size_t limit = 10) const
    {
      LockGuard lock(*this);

      std::vector<ScoredWord> scored;
      std::string current;
      collect_scored(root_.get(), current, scored, query);

      std::sort(scored.begin(), scored.end(), [](const auto &a, const auto &b)
                {
        if (a.score != b.score) return a.score > b.score;
        return a.word < b.word; });

      const std::size_t take = (limit == 0) ? scored.size() : std::min(limit, scored.size());

      std::vector<std::string> out;
      out.reserve(take);

      for (std::size_t i = 0; i < take; ++i)
      {
        out.push_back(scored[i].word);
      }
      return out;
    }

  private:
    struct ScoredWord final
    {
      std::string word;
      double score{0.0};
    };

    class LockGuard final
    {
    public:
      explicit LockGuard(const Trie &t)
          : t_(t)
      {
        if (t_.thread_safe_)
        {
          t_.mtx_.lock();
          locked_ = true;
        }
      }

      ~LockGuard()
      {
        if (locked_)
        {
          t_.mtx_.unlock();
        }
      }

      LockGuard(const LockGuard &) = delete;
      LockGuard &operator=(const LockGuard &) = delete;

    private:
      const Trie &t_;
      bool locked_{false};
    };

    static void collect_suggestions(
        const TrieNode *node,
        std::string &current,
        std::vector<std::string> &out,
        std::size_t limit)
    {
      if (node->is_terminal)
      {
        out.push_back(current);
        if (limit != 0 && out.size() >= limit)
        {
          return;
        }
      }

      for (const auto &kv : node->children)
      {
        current.push_back(kv.first);
        collect_suggestions(kv.second.get(), current, out, limit);
        current.pop_back();

        if (limit != 0 && out.size() >= limit)
        {
          return;
        }
      }
    }

    static int levenshtein_distance(const std::string &a, const std::string &b)
    {
      const std::size_t m = a.size();
      const std::size_t n = b.size();

      if (m == 0)
        return static_cast<int>(n);
      if (n == 0)
        return static_cast<int>(m);

      std::vector<int> prev(n + 1);
      std::vector<int> cur(n + 1);

      for (std::size_t j = 0; j <= n; ++j)
      {
        prev[j] = static_cast<int>(j);
      }

      for (std::size_t i = 1; i <= m; ++i)
      {
        cur[0] = static_cast<int>(i);
        for (std::size_t j = 1; j <= n; ++j)
        {
          const int cost = (a[i - 1] == b[j - 1]) ? 0 : 1;
          cur[j] = std::min({prev[j] + 1, cur[j - 1] + 1, prev[j - 1] + cost});
        }
        std::swap(prev, cur);
      }

      return prev[n];
    }

    static double score_word(const std::string &query, const std::string &word, std::uint32_t frequency)
    {
      const int d = levenshtein_distance(query, word);

      const double sim = 1.0 / (1.0 + static_cast<double>(d));
      const double len_bonus = static_cast<double>(word.size()) * 0.02;
      const double freq_bonus = static_cast<double>(frequency) * 0.05;

      return sim + len_bonus + freq_bonus;
    }

    static void collect_scored(
        const TrieNode *node,
        std::string &current,
        std::vector<ScoredWord> &out,
        const std::string &query)
    {
      if (node->is_terminal)
      {
        out.push_back(ScoredWord{current, score_word(query, current, node->frequency)});
      }

      for (const auto &kv : node->children)
      {
        current.push_back(kv.first);
        collect_scored(kv.second.get(), current, out, query);
        current.pop_back();
      }
    }

  private:
    std::unique_ptr<TrieNode> root_;
    bool thread_safe_{false};
    mutable std::mutex mtx_;
  };

} // namespace trie

#endif // TRIE_TRIE_HPP
