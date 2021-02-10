// Copyright 2021 Roger Peralta Aranibar
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <random>
#include <sstream>
#include <thread>

#include "b-link.hpp"

std::size_t NUMBER_THREADS = 8;

const std::size_t NUMBER_OPERATIONS = 10;

int MIN_VALUE = 1;
int MAX_VALUE = 10;

std::random_device rd;

class TreeSearcher {
public:
    TreeSearcher(unsigned int id, EDA::Concurrent::BLinkTree<3, int> *b_link)
            : id_(id), b_link_(b_link) {}

    void operator()() {
        std::uniform_int_distribution<int> distribution(MIN_VALUE, MAX_VALUE);
        for (std::size_t i = 0; i < NUMBER_OPERATIONS; ++i) {
            std::cout << distribution(rd) << "\n";
            // b_link_->search(distribution(rd));
        }
    }

private:
    unsigned int id_;
    EDA::Concurrent::BLinkTree<3, int> *b_link_;
};

void sequential_insert(EDA::Concurrent::BLinkTree<3, int> *b_link) {
    std::uniform_int_distribution<int> distribution(MIN_VALUE, MAX_VALUE);
    for (std::size_t i = 0; i < NUMBER_OPERATIONS; ++i) {
        std::cout << distribution(rd) << "\n";
        // b_link->insert(distribution(rd));
    }
}

void run_test() {
    EDA::Concurrent::BLinkTree<3, int> b_link;

    sequential_insert(&b_link);

    std::thread *threads = new std::thread[NUMBER_THREADS];

    for (std::size_t i = 0; i < NUMBER_THREADS; ++i) {
        threads[i] = std::thread(TreeSearcher(i, &b_link));
    }
    for (std::size_t i = 0; i < NUMBER_THREADS; ++i) {
        threads[i].join();
    }
}

void parallel_insert(EDA::Concurrent::BLinkTree<3, int> *b_link) {
    std::uniform_int_distribution<int> distribution(MIN_VALUE, MAX_VALUE);
    for (std::size_t i = 0; i < NUMBER_OPERATIONS; ++i) {
        int value = distribution(rd);
        std::cout << value << "\n";
        // b_link->insert(value);
    }
}

void parallel_search(EDA::Concurrent::BLinkTree<3, int> *b_link) {
    std::uniform_int_distribution<int> distribution(MIN_VALUE, MAX_VALUE);
    for (std::size_t i = 0; i < NUMBER_OPERATIONS; ++i) {
        int value = distribution(rd);
        std::cout << value << "\n";
        // b_link->search(value);
    }
}

void run_parallel_asynchronous_test() {
    EDA::Concurrent::BLinkTree<3, int> b_link;

    std::thread *insert_threads = new std::thread[NUMBER_THREADS];
    std::thread *search_threads = new std::thread[NUMBER_THREADS];

    for (std::size_t i = 0; i < NUMBER_THREADS; ++i) {
        insert_threads[i] = std::thread(parallel_insert, &b_link);
        search_threads[i] = std::thread(parallel_search, &b_link);
    }
    for (std::size_t i = 0; i < NUMBER_THREADS; ++i) {
        insert_threads[i].join();
        search_threads[i].join();
    }
}

int *LAST_VALUE_INSERTED;
std::mutex *MUTEX;
std::condition_variable *VALUE_INSERTED;

void insert_and_notify(EDA::Concurrent::BLinkTree<3, int> *b_link, int id) {
    std::uniform_int_distribution<int> distribution(MIN_VALUE, MAX_VALUE);
    for (std::size_t i = 0; i < NUMBER_OPERATIONS; ++i) {
        std::unique_lock<std::mutex> lock(MUTEX[id]);
        int value = distribution(rd);
         b_link->insert(value);
        LAST_VALUE_INSERTED[id] = value;
        VALUE_INSERTED[id].notify_one();
    }
    std::stringstream stream;
    stream << "Insert finished for thread: " << id << "\n";
    std::cout << stream.str() << "\n";
}

void search_when_notified(EDA::Concurrent::BLinkTree<3, int> *b_link, int id) {
    while (true) {
        std::unique_lock<std::mutex> lock(MUTEX[id]);
        VALUE_INSERTED[id].wait(lock);
        std::cout << LAST_VALUE_INSERTED[id] << "\n";
        if (!b_link->search(LAST_VALUE_INSERTED[id])) {
           std::stringstream stream;
           stream << "Value not found in B Link: " << LAST_VALUE_INSERTED[id]
                  << "\n";
           std::cerr << stream.str() << "\n";
         }
    }
}

void run_parallel_synchronous_test() {
    LAST_VALUE_INSERTED = new int[NUMBER_THREADS];
    MUTEX = new std::mutex[NUMBER_THREADS];
    VALUE_INSERTED = new std::condition_variable[NUMBER_THREADS];

    EDA::Concurrent::BLinkTree<3, int> b_link;

    std::thread *insert_threads = new std::thread[NUMBER_THREADS];
    std::thread *search_threads = new std::thread[NUMBER_THREADS];

    for (std::size_t i = 0; i < NUMBER_THREADS; ++i) {
        insert_threads[i] = std::thread(insert_and_notify, &b_link, i);
        search_threads[i] = std::thread(search_when_notified, &b_link, i);
    }
    for (std::size_t i = 0; i < NUMBER_THREADS; ++i) {
        insert_threads[i].join();
        // do not uncomment the following line
         search_threads[i].join();
    }
}

int main() {
    run_parallel_asynchronous_test();
    run_parallel_synchronous_test();
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////

/*// Copyright 2021 Roger Peralta Aranibar
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <random>
#include <thread>

#include "b-link.hpp"

std::size_t NUMBER_THREADS = 8;

const std::size_t NUMBER_OPERATIONS = 10;

int MIN_VALUE = 1;
int MAX_VALUE = 10;

std::random_device rd;

class TreeSearcher {
 public:
  TreeSearcher(unsigned int id, EDA::Concurrent::BLinkTree<3, int> *b_link)
      : id_(id), b_link_(b_link) {}

  void operator()() {
    std::uniform_int_distribution<int> distribution(MIN_VALUE, MAX_VALUE);
    for (std::size_t i = 0; i < NUMBER_OPERATIONS; ++i) {
      std::cout << distribution(rd) << "\n";//comentar
       b_link_->search(distribution(rd));
    }
  }

 private:
  unsigned int id_;
  EDA::Concurrent::BLinkTree<3, int> *b_link_;
};

void sequential_insert(EDA::Concurrent::BLinkTree<3, int> *b_link) {
  std::uniform_int_distribution<int> distribution(MIN_VALUE, MAX_VALUE);
  for (std::size_t i = 0; i < NUMBER_OPERATIONS; ++i) {
    std::cout << distribution(rd) << "\n";//comentar
    b_link->insert(distribution(rd));
  }
}

void run_test() {
  EDA::Concurrent::BLinkTree<3, int> b_link;

 *//* sequential_insert(&b_link);

  std::thread *threads = new std::thread[NUMBER_THREADS];

  for (std::size_t i = 0; i < NUMBER_THREADS; ++i) {
    threads[i] = std::thread(TreeSearcher(i, &b_link));
  }
  for (std::size_t i = 0; i < NUMBER_THREADS; ++i) {
    threads[i].join();
  }*//*
    b_link.insert(5);
    b_link.insert(6);
    b_link.insert(69);
    b_link.insert(9);
    b_link.insert(11);
    b_link.insert(19);
    b_link.insert(1);
    b_link.insert(4986513);
    b_link.insert(65);
    b_link.search(19);
}

int main() {
  run_test();
  return 0;
}*/
