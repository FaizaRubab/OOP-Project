#pragma once
template <typename T>
class Storage 
{
private:
    T data[100];
    int count;
public:
    Storage() : count(0) {}
    ~Storage() {}

    // Add an element (returns false if full)
    bool add(const T& item) 
    {
        if (count >= 100)
        {
            return false;
        }
        data[count++] = item;
        return true;
    }

    // Remove by index 
    bool removeAt(int index)
    {
        if (index < 0 || index >= count)
        {
            return false;
        }
        for (int i = index; i < count - 1; ++i)
        {
            data[i] = data[i + 1];
        }
        --count;
        return true;
    }

    T* findById(int id)
    {
        for (int i = 0; i < count; ++i)
        {
            if (data[i].getId() == id)
                return &data[i];
        }
        return nullptr;
    }

    int indexById(int id) const 
    {
        for (int i = 0; i < count; ++i)
        {
            if (data[i].getId() == id)
                return i;
        }
        return -1;
    }

    // Remove by ID
    bool removeById(int id)
    {
        int idx = indexById(id);
        if (idx < 0) 
            return false;
        return removeAt(idx);
    }

    T* getAll() 
    {
        return data;
    }
    const T* getAll() const
    {
        return data; 
    }
    int size() const 
    {
        return count;
    }
    T& operator[](int i)      
    {
        return data[i]; 
    }
    const T& operator[](int i) const 
    {
        return data[i];
    }
    int maxId() const
    {
        int m = 0;
        for (int i = 0; i < count; ++i)
        {
            if (data[i].getId() > m)
                m = data[i].getId();
        }
        return m;
    }
    // Clear all
    void clear() 
    {
        count = 0;
    }
};