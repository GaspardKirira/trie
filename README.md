# trie

Minimal prefix tree (Trie) for modern C++.

`trie` provides deterministic autocomplete and ranked fuzzy search
without hidden behavior.

Header-only. Zero dependencies.

## Download

https://vixcpp.com/registry/pkg/gaspardkirira/trie

## Why trie?

Prefix trees are useful in:

-   Autocomplete systems
-   Search suggestions
-   Command-line interfaces
-   Dictionaries
-   Spell correction
-   Fuzzy matching

This library provides:

-   Word insertion
-   Exact lookup
-   Prefix suggestions
-   Ranked fuzzy search (Levenshtein-based)
-   Optional thread-safety

No dynamic polymorphism.\
No hidden state.\
No global variables.

Just a minimal deterministic Trie.

## Installation

### Using Vix Registry

``` bash
vix add gaspardkirira/trie
vix deps
```

### Manual

``` bash
git clone https://github.com/GaspardKirira/trie.git
```

Add the `include/` directory to your project.

## Quick Examples

### Basic Usage

``` cpp
#include <trie/trie.hpp>
#include <iostream>

int main()
{
  trie::Trie t;

  t.insert("alice");
  t.insert("bob");

  std::cout << std::boolalpha;
  std::cout << t.contains("alice") << "\n";
  std::cout << t.contains("eve") << "\n";
}
```

### Autocomplete

``` cpp
#include <trie/trie.hpp>
#include <iostream>

int main()
{
  trie::Trie t;

  t.insert("apple");
  t.insert("app");
  t.insert("application");

  auto suggestions = t.suggest("app");

  for (const auto& word : suggestions)
  {
    std::cout << word << "\n";
  }
}
```

### Ranked Fuzzy Search

``` cpp
#include <trie/trie.hpp>
#include <iostream>

int main()
{
  trie::Trie t;

  t.insert("hello");
  t.insert("hallo");
  t.insert("hullo");
  t.insert("world");

  auto results = t.search_ranked("helo", 3);

  for (const auto& word : results)
  {
    std::cout << word << "\n";
  }
}
```

## API Overview

`trie::Trie`

-   `Trie::insert(word)`
-   `Trie::contains(word)`
-   `Trie::suggest(prefix, limit = 0)`
-   `Trie::search_ranked(query, limit = 10)`

Constructor:

``` cpp
trie::Trie(bool thread_safe = false);
```

If `thread_safe` is true, operations are protected by an internal mutex.

## Design Principles

-   Explicit over implicit
-   Deterministic scoring
-   No hidden caching
-   No global state
-   Minimal surface area
-   Header-only integration

This library is intentionally small.

If you need:

-   Unicode-aware grapheme segmentation
-   Custom ranking pipelines
-   Persistent storage
-   Distributed indexing

Build those on top.

## Performance Notes

-   Insert and lookup are **O(k)**, where **k** is word length
-   Prefix suggestion traverses only matching branches
-   Ranked search scans all terminal nodes (**O(n)**) and applies
    deterministic scoring

Designed for small to medium datasets where simplicity and clarity
matter.

## Tests

Run:

``` bash
vix build
vix tests
```

Tests verify:

-   Insert / contains correctness
-   Prefix suggestions
-   Limit behavior
-   Ranked search stability
-   Thread-safe mode (basic)

## License

MIT License
Copyright (c) Gaspard Kirira

