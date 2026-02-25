#include <trie/trie.hpp>

#include <cassert>
#include <string>
#include <vector>

static void test_insert_and_contains()
{
  trie::Trie t;

  t.insert("alice");
  t.insert("alicia");
  t.insert("bob");

  assert(t.contains("alice"));
  assert(t.contains("alicia"));
  assert(t.contains("bob"));

  assert(!t.contains("ali"));
  assert(!t.contains("bobby"));
  assert(!t.contains(""));
}

static void test_suggest_basic()
{
  trie::Trie t;

  t.insert("alice");
  t.insert("alicia");
  t.insert("ali");
  t.insert("bob");

  const auto s1 = t.suggest("ali");
  assert(!s1.empty());

  bool has_ali = false;
  bool has_alice = false;
  bool has_alicia = false;

  for (const auto &w : s1)
  {
    if (w == "ali")
      has_ali = true;
    if (w == "alice")
      has_alice = true;
    if (w == "alicia")
      has_alicia = true;
  }

  assert(has_ali);
  assert(has_alice);
  assert(has_alicia);

  const auto s2 = t.suggest("zzz");
  assert(s2.empty());
}

static void test_suggest_limit()
{
  trie::Trie t;

  t.insert("a");
  t.insert("aa");
  t.insert("aaa");
  t.insert("aaaa");

  const auto s = t.suggest("a", 2);
  assert(s.size() == 2);
}

static void test_search_ranked()
{
  trie::Trie t;

  t.insert("hello");
  t.insert("hallo");
  t.insert("hullo");
  t.insert("world");

  const auto r = t.search_ranked("helo", 3);
  assert(!r.empty());
  assert(r.size() <= 3);

  // Should not return unrelated word first in typical cases.
  assert(r[0] != "world");
}

static void test_thread_safe_flag_smoke()
{
  trie::Trie t(true);
  t.insert("abc");
  assert(t.contains("abc"));
}

int main()
{
  test_insert_and_contains();
  test_suggest_basic();
  test_suggest_limit();
  test_search_ranked();
  test_thread_safe_flag_smoke();
  return 0;
}
