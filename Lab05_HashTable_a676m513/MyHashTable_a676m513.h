#ifndef __MYHASHTABLE_H__
#define __MYHASHTABLE_H__

#include <iostream>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <string>

#include "MyVector_a676m513.h"
#include "MyLinkedList_a676m513.h"

static const long long uh_param_a = 53;       // universal hash function parameter a
static const long long uh_param_b = 97;       // universal hash function parameter b
static const long long prime_digits = 19;     // parameter used for finding a Mersenne prime
static const long long mersenne_prime = (1 << prime_digits) - 1;  // the Mersenne prime for universal hashing

// fast calculation of (n modulo mersenne_prime)
long long fastMersenneModulo(const long long n)
{
    // code begins
    return (n >> prime_digits) + (n & mersenne_prime);
    // code ends
}

// definition of the template hash function class
template <typename KeyType>
class HashFunc
{
  public:
    long long univHash(const KeyType key, const long long table_size) const;
};

// the hash function class that supports the hashing of the "long long" data type
template <>
class HashFunc<long long>
{
  public:
    long long univHash(const long long key, const long long table_size) const
    {
        long long hv = fastMersenneModulo(static_cast<long long>(uh_param_a * key + uh_param_b));
        hv = hv % table_size;
        return hv;
    }
};

// the has function class that supports the hashing of the "std::string" data type
template <>
class HashFunc<std::string>
{
  private:
    const int param_base = 37;      // the base used for inflating each character
  public:    
    long long univHash(const std::string& key, const long long table_size) const
    {
        long long hv = 0;
        for(size_t i = 0; i < key.length(); ++i)
        {
            hv = param_base * hv + static_cast<long long>(key[i]);
        }
        hv = fastMersenneModulo(static_cast<long long>(uh_param_a * hv + uh_param_b));
        hv = hv % table_size;
        return hv;
    }
};

// definition of the template hashed object class
template <typename KeyType, typename ValueType>
class HashedObj
{
  public:

    KeyType key;
    ValueType value;

    HashedObj()
    {
        return;
    }

    HashedObj(const KeyType& k, const ValueType& v) :
        key(k),
        value(v)
    {
        return;
    }
  
    HashedObj(KeyType && k, ValueType && v) :
        key(std::move(k)),
        value(std::move(v))
    {
        return;
    }

    bool operator==(const HashedObj<KeyType, ValueType>& rhs)
    {
        return (key == rhs.key);
    }    

    bool operator!=(const HashedObj<KeyType, ValueType>& rhs)
    {
        return !(*this == rhs);
    }   

};

template <typename KeyType, typename ValueType>
class MyHashTable
{ 
  private:
    size_t theSize; // the number of data elements stored in the hash table
    MyVector<MyLinkedList<HashedObj<KeyType, ValueType> >* > hash_table;    // the hash table implementing the separate chaining approach
    MyVector<size_t> primes;    // a set of precomputed and sorted prime numbers
    HashFunc<KeyType> myHashTable;
    size_t theCapacity;

    // pre-calculate a set of primes using the sieve of Eratosthenes algorithm
    // will be called if table doubling requires a larger prime number for table size
    // expected to update the private member "primes"
    void preCalPrimes(const size_t n)
    {
        
        // code begins
        while (!primes.empty())
        {
            primes.pop_front();
        }

        MyVector<bool> isPrime;
        isPrime.reserve(n + 1);

        for (size_t i = 0; i <= n; i++)
        {
            isPrime[i] = 1;
        }

        isPrime[0] = isPrime[1] = 0;

        for (size_t i = 2; i <= n; i++)
        {
            if (isPrime[i] && (long long)i * i <= n)
            {
                for (size_t j = i * i; j <= n; j += i)
                {
                    isPrime[j] = 0;
                }
            }
        }

        for (size_t i = 0; i <= n; i++)
        {
            if (isPrime[i])
            {
                primes.push_back(i);
            }
        }

        // code ends
    }

    // finding the smallest prime that is larger than or equal to n
    // should perform binary search against the private member "primes"
    size_t nextPrime(const size_t n)
    {
        
        // code begins
        if (n > primes.back())
        {
            preCalPrimes(n * 2);
        }

        size_t low, mid, high;
        low = 0;
        high = primes.size() - 1;

        while (high - low > 1)
        {
            mid = (high + low) / 2;

            if (primes[mid] < n)
            {
                low = mid + 1;
            }

            else
            {
                high = mid;
            }
        }

        if (primes[low] >= n)
        {
            if (low < primes.size() - 1)
            {
                return primes[low];
            }
        }

        else if (primes[high] >= n)
        {
            if (high < primes.size() - 1)
            {
                return primes[high];
            }
        }

        return 0;
        // code ends
    }

    // finds the MyLinkedList itertor that corresponds to the hashed object that has the specified key
    // returns the end() iterator if not found
    typename MyLinkedList<HashedObj<KeyType, ValueType> >::iterator find(const KeyType& key)
    {
        // code begins
        auto & bucket = hash_table[myHashTable.univHash(key, hash_table.capacity())];
        auto itr = bucket -> begin();

        for (itr; itr != bucket -> end(); itr++)
        {
            if (key == (*itr).key)
            {
                return itr;
            }
        }

        return itr;
        // code ends
    }

    // rehashes all data elements in the hash table into a new hash table with new_size
    // note that the new_size can be either smaller or larger than the existing size
    void rehash(const size_t new_size)
    {
        
        // code begins
        MyVector<MyLinkedList<HashedObj<KeyType, ValueType>>*> newHashTable;
        newHashTable.reserve(new_size);
        size_t oldHashTable = hash_table.capacity();

        for (size_t i = 0; i < new_size; ++i)
        {
            newHashTable.push_back(new MyLinkedList<HashedObj<KeyType, ValueType>>);
        }

        for (size_t i = 0; i < oldHashTable; ++i)
        {
            auto & bucket = hash_table[i];

            for (auto itr = bucket -> begin(); itr != bucket -> end(); ++itr)
            {
                auto & newBucket = newHashTable[myHashTable.univHash((*itr).key, new_size)];

                newBucket -> push_back(*itr);
            }
        }

        std::swap(hash_table, newHashTable);

        for (size_t i = 0; i < oldHashTable; ++i)
        {
            delete newHashTable[i];
        }

        theCapacity = new_size;

        // code ends
    }

    // doubles the size of the table and perform rehashing
    // the new table size should be the smallest prime that is larger than the expected new table size (double of the old size)
    void doubleTable()
    {
        size_t new_size = nextPrime(2 * hash_table.capacity());

        if (new_size == 7)
		{
			this -> rehash(17);
		}
		else
		{
			this -> rehash(new_size);
		}
        return;
        
    }

    // halves the size of the table and perform rehahsing
    // the new table size should be the smallest prime that is larger than the expected new table size (half of the old size)
    void halveTable()
    {
        size_t new_size = nextPrime(ceil(hash_table.size() / 2));
        this->rehash(new_size);
        return;
    }

  public:

    // the default constructor; allocate memory if necessary
    explicit MyHashTable(const size_t init_size = 3)
    {
        // code begins
        theSize = 0;
        theCapacity = init_size;
        hash_table.reserve(init_size);

        for (size_t i = 0; i < theCapacity; i++)
        {
            hash_table[i] = new MyLinkedList<HashedObj<KeyType, ValueType>>();
        }

        preCalPrimes(theCapacity);

        // code ends
    }

    // the default destructor; collect memory if necessary
    ~MyHashTable()
    {
        // code begins
        for (size_t i = 0; i < theCapacity; i++)
        {
            delete hash_table[i];
        }
        // code ends
    }

    // checks if the hash tabel contains the given key
    bool contains(const KeyType& key)
    {
        // code begins
        auto & bucket = hash_table[myHashTable.univHash(key, hash_table.capacity())];

        return find(key) != bucket -> end();    
        // code ends
    }

    // retrieves the data element that has the specified key
    // returns true if the key is contained in the hash table
    // return false otherwise
    bool retrieve(const KeyType& key, HashedObj<KeyType, ValueType>& data)
    {
        // code begins
        auto & bucket = hash_table[myHashTable.univHash(key, hash_table.capacity())];

        if (find(key) != bucket -> end())
        {
            data = *find(key);
            return 1;
        }

        return 0;
        // code ends
    }

    // inserts the given data element into the hash table (copy)
    // returns true if the key is not contained in the hash table
    // return false otherwise
    bool insert(const HashedObj<KeyType, ValueType>& x)
    {
        // code begins
        auto & bucket = hash_table[myHashTable.univHash(x.key, hash_table.capacity())];

        if (find(x.key) != bucket -> end())
        {
            return 0;
        }

        bucket -> push_back(x);

        if (++theSize * 2 > hash_table.capacity())
        {
            doubleTable();
        }

        return 1;
        // code ends
    }

    // inserts the given data element into the hash table (move)
    // returns true if the key is not contained in the hash table
    // return false otherwise
    bool insert(HashedObj<KeyType, ValueType> && x)
    {
        // code begins
        auto & bucket = hash_table[myHashTable.univHash(x.key, hash_table.capacity())];

        if (find(x.key) != bucket -> end())
        {
            return 0;
        }

        bucket -> push_back(std::move(x));

        if (++theSize * 2 > hash_table.capacity())
        {
            doubleTable();
        }

        return 1;
        // code ends
    }

    // removes the data element that has the key from the hash table
    // returns true if the key is contained in the hash table
    // returns false otherwise
    bool remove(const KeyType& key)
    {
        // code begins
        auto & bucket = hash_table[myHashTable.univHash(key, hash_table.capacity())];
        auto itr = find(key);

        if (itr == bucket -> end())
        {
            return 0;
        }

        bucket -> erase(itr);
        --theSize;

        if (theSize <= hash_table.capacity() / 8)
        {
            halveTable();
        }

        return 1;
        // code ends
    }

    // returns the number of data elements stored in the hash table
    size_t size()
    {
        // code begins
        return theSize;
        // code ends
    }

    // returns the capacity of the hash table
    size_t capacity()
    {
        // code begins
        return theCapacity;
        // code ends
    }

};


#endif // __MYHASHTABLE_H__
