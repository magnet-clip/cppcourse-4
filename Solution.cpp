#include "Common.h"

#include <iostream>
#include <list>
#include <map>
#include <mutex>
#include <unordered_map>

using namespace std;

class LruCache : public ICache {
public:
  LruCache(shared_ptr<IBooksUnpacker> books_unpacker, const Settings &settings)
      : _unpacker(books_unpacker), _cache({settings.max_memory}) {}

  BookPtr GetBook(const string &book_name) override {
    lock_guard guard(_cache_mutex);
    auto book_reference = _cache.Find(book_name);
    if (!book_reference.has_value()) {
      auto unpacked_book = _unpacker->UnpackBook(book_name);
      // converting unique pointer to const shared one
      auto book = BookPtr(unpacked_book.release());
      _cache.Add(book);
      return book;
    } else {
      _cache.Up(book_reference->iterator);
      return book_reference->pointer;
    }
  }

private:
  struct Cache {
    using PriorityCache = list<BookPtr>;
    struct BookReference {
      BookPtr pointer;
      PriorityCache::iterator iterator;
    };

    using NameCache = unordered_map<string, BookReference>;

    explicit Cache(size_t memory_limit) : _memory_limit(memory_limit) {}

    optional<BookReference> Find(const string &name) {
      auto it = _name.find(name);
      if (it != _name.end()) {
        return it->second;
      }
      return {};
    }

    void Add(const BookPtr &book) {
      _total_memory += book->GetContent().length();
      _priority.push_back(book);
      _name[book->GetName()] = {book, prev(_priority.end())};
      Cleanup();
    }

    void Up(PriorityCache::iterator it) {
      // move this book to end of queue
      _priority.splice(_priority.end(), _priority, it);
    }

  private:
    void Cleanup() {
      while (_total_memory > _memory_limit) {
        auto book = _priority.front();
        _total_memory -= book->GetContent().length();
        _priority.pop_front();
        _name.erase(book->GetName());
      }
    }

    PriorityCache _priority;
    NameCache _name;
    size_t _memory_limit = 0;
    size_t _total_memory = 0;
  };

  shared_ptr<IBooksUnpacker> _unpacker;

  mutex _cache_mutex;
  Cache _cache;
};

unique_ptr<ICache> MakeCache(shared_ptr<IBooksUnpacker> books_unpacker,
                             const ICache::Settings &settings) {
  return unique_ptr<ICache>(new LruCache(books_unpacker, settings));
}
