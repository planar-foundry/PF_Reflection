#include <PF_Test/UnitTest.hpp>
#include <PF_Debug/Log.hpp>

#include <array>
#include <string>
#include <unordered_map>
#include <vector>

namespace pf::reflection
{
    class Field
    {
    public:
        Field() {}
        Field(const char* name, const char* type, int offset, int size, int alignment)
            : m_name(name),
              m_type(type),
              m_offset(offset),
              m_size(size),
              m_alignment(alignment)
        { }

        const std::string& name() const { return m_name; }
        const std::string& type() const { return m_type; }
        int offset() const { return m_offset; }
        int size() const { return m_size; }
        int alignment() const { return m_alignment; }

        template <typename T, typename U>
        const T& get(const U& instance)
        {
            // assert sizeof(T) == m_size
            T* ptr = get_instance_ptr<T>(&instance, m_offset);
            return *ptr;
        }

        template <typename T, typename U>
        void set(const U& instance, T&& value)
        {
            // assert sizeof(T) == m_size
            T* ptr = get_instance_ptr<T>(&instance, m_offset);
            *ptr = std::forward<T>(value);
        }

    private:
        std::string m_name;
        std::string m_type;
        int m_offset;
        int m_size;
        int m_alignment;

        template <typename T, typename U>
        static constexpr T* get_instance_ptr(U* instance, int offset)
        {
            std::byte* instance_offset = (std::byte*)instance;
            instance_offset += offset;
            return (T*)instance_offset;
        }
    };

    template <typename T>
    struct FieldIntrospector
    {
        using iterator = decltype(T::_pf_reflection_fields)::template iterator;
        static iterator begin() { return std::begin(T::_pf_reflection_fields); }
        static iterator end() { return std::end(T::_pf_reflection_fields); }
    };

    template <typename T>
    struct FieldProxy
    {
        FieldProxy(FieldIntrospector<T>::template iterator begin,
            FieldIntrospector<T>::template iterator end)
            : m_begin(begin), m_end(end)
        { }

        FieldIntrospector<T>::template iterator begin() { return m_begin; }
        FieldIntrospector<T>::template iterator end() { return m_end; }

    private:

        FieldIntrospector<T>::template iterator m_begin;
        FieldIntrospector<T>::template iterator m_end;
    };


    template <typename T>
    FieldProxy<T> get_fields()
    {
        return { FieldIntrospector<T>::begin(), FieldIntrospector<T>::end() };
    }
}

template <int N>
struct FieldIndex {};

template <typename T, int N>
struct RegisterClass
{
    RegisterClass()
    {
        if (!m_registered)
        {
            T::template _pf_reflection_register_field(FieldIndex<N - 1>());
            RegisterClass<T, N - 1>();
            m_registered = true;
        }
    }

    static inline bool m_registered = false;
};

template <typename T>
struct RegisterClass<T, 0>
{
    RegisterClass() {}
};

#define CLS_BEGIN(name) \
    template <int> static void _pf_reflection_register_field(); \
    using _pf_type = name; \
    constexpr static int _pf_field_count_before = __COUNTER__;

#define FIELD(type, name) \
    type name; \
    constexpr static int _pf_reflection_field_##name##_idx = __COUNTER__ - _pf_field_count_before - 1; \
    static void _pf_reflection_register_field(FieldIndex<_pf_reflection_field_##name##_idx>) \
    { \
        _pf_reflection_fields[_pf_reflection_field_##name##_idx] = { #name, #type, offsetof(_pf_type, name), sizeof(name), alignof(type) }; \
        _pf_reflection_field_map[#name] = _pf_reflection_field_##name##_idx; \
    }

#define CLS_END() \
    constexpr static int _pf_field_count_after = __COUNTER__; \
    constexpr static int _pf_field_count = _pf_field_count_after - _pf_field_count_before - 1; \
    static inline std::array<pf::reflection::Field, _pf_field_count> _pf_reflection_fields; /* TODO: maybe can be fully compile time */ \
    static inline std::unordered_map<std::string, int> _pf_reflection_field_map; \
    static inline RegisterClass<TestComponent, _pf_field_count> _pf_reflection_registrar; \
    template <typename T, int N> friend struct RegisterClass; \
    template <typename T> friend struct pf::reflection::FieldIntrospector;

struct TestComponent
{
    int blah; // non-reflected

    CLS_BEGIN(TestComponent)
        FIELD(int, m_count)
        FIELD(int, m_count2)
        FIELD(int, m_count3)
        FIELD(std::vector<int>, m_numbers)
    CLS_END()
};

PFTEST_CREATE(Manual)
{
    TestComponent instance;
    instance.m_count = 5;
    instance.m_count2 = 10;
    instance.m_count3 = 15;
    instance.m_numbers.emplace_back(20);
    instance.m_numbers.emplace_back(25);
    instance.m_numbers.emplace_back(30);

    for (pf::reflection::Field& field : pf::reflection::get_fields<TestComponent>())
    {
        PFDEBUG_LOG_DEBUG("%s %s", field.type().c_str(), field.name().c_str());

        if (field.type() == "int")
        {
            PFDEBUG_LOG_DEBUG("%d", field.get<int>(instance));
        }
        else if (field.type() == "std::vector<int>")
        {
            for (int num : field.get<std::vector<int>>(instance))
            {
                PFDEBUG_LOG_DEBUG("%d ", num);
            }
        }

        PFDEBUG_LOG_DEBUG("\n");
    }
}
