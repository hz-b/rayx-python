#pragma once

#include <Variant.h>
#include <pybind11/pybind11.h>

#include <concepts>
#include <functional>
#include <tuple>

namespace py = pybind11;

namespace reflect {

template <typename T>
struct Ref {
    std::function<const T()> get;
    std::function<void(T)> set;

    Ref& operator=(const T& value) {
        set(value);
        return *this;
    }

    Ref& operator=(const Ref& other) {
        set(other.get());
        return *this;
    }
};

template <typename S, typename M>
struct field_info {
    using StructType = S;
    using MemberType = M;

    M S::* member;
    const char* name;
    std::optional<M> default_value;
};

template <typename S, typename M>
struct prop_info {
    using StructType = S;
    using MemberType = M;

    M (S::*getter)() const;
    void (S::*setter)(M);
    const char* name;
};

template <typename T>
struct info;

template <typename T>
concept Structure = requires {
    typename info<T>;
    { info<T>::type_name } -> std::convertible_to<const char*>;
    { info<T>::fields };
};

struct any_callable {
  public:
    template <typename T>
    void operator()(T&&) {}
};

template <typename T>
concept Variant = requires(T v) {
    typename info<T>;
    { info<T>::type_name } -> std::convertible_to<const char*>;
    v.visit(any_callable{});
};

template <typename S, typename M>
void bind(py::class_<S>& cls, const field_info<S, M>& field) {
    cls.def_property(
        field.name,
        [field](S& self) {
            if constexpr (Structure<M>) {
                return py::cast(Ref<M>{[&self, field]() { return self.*(field.member); }, [&self, field](M value) { self.*(field.member) = value; }});
            } else if constexpr (Variant<M>) {
                M m = self.*(field.member);
                return m.visit([field, &self]<typename U>(U&& value) {
                    return py::cast(Ref<std::remove_cvref_t<U>>{
                        [&self, field] { return self.*(field.member).template get<std::remove_cvref_t<U>>(); },
                        [&self, field](U new_value) { self.*(field.member).template get<std::remove_cvref_t<U>>() = new_value; }});
                });
            }
            return py::cast(self.*(field.member));
        },
        [field](S& self, M m) { self.*(field.member) = m; });
}

template <typename S, typename M>
void bind(py::class_<S>& cls, const prop_info<S, M>& prop) {
    cls.def_property(
        prop.name,
        [prop](S& self) {
            if constexpr (Structure<M>) {
                return py::cast(
                    Ref<M>{[&self, prop]() { return (self.*(prop.getter))(); }, [&self, prop](M value) { (self.*(prop.setter))(value); }});
            } else if constexpr (Variant<M>) {
                M m = (self.*(prop.getter))();
                return m.visit([prop, &self]<typename U>(U&& value) {
                    return py::cast(Ref<std::remove_cvref_t<U>>{[&self, prop] {
                                                                    M m_local = (self.*(prop.getter))();
                                                                    return m_local.template get<std::remove_cvref_t<U>>();
                                                                },
                                                                [&self, prop](U new_value) {
                                                                    M m_local = (self.*(prop.getter))();
                                                                    m_local.template get<std::remove_cvref_t<U>>() = new_value;
                                                                    (self.*(prop.setter))(m_local);
                                                                }});
                });
            }
            return py::cast((self.*(prop.getter))());
        },
        [prop](S& self, M m) { (self.*(prop.setter))(m); });
}

template <typename S, typename M>
void bind_ref(py::class_<Ref<S>>& cls, const field_info<S, M>& field) {
    cls.def_property(
        field.name,
        [field](Ref<S>& self) {
            if constexpr (Structure<M>) {
                return py::cast(Ref<M>{[&self, field]() { return self.get().*(field.member); },
                                       [&self, field](M value) {
                                           S s = self.get();
                                           s.*(field.member) = value;
                                           self.set(s);
                                       }});
            } else if constexpr (Variant<M>) {
                S s = self.get();
                M m = s.*(field.member);
                return m.visit([field, &self, s]<typename U>(U&& value) {
                    return py::cast(Ref<std::remove_cvref_t<U>>{[&self, field] {
                                                                    S s_local = self.get();
                                                                    return s_local.*(field.member);
                                                                },
                                                                [&self, field](U new_value) {
                                                                    S s_local = self.get();
                                                                    s_local.*(field.member) = new_value;
                                                                    self.set(s_local);
                                                                }});
                });
            }
            S s = self.get();
            return py::cast(s.*(field.member));
        },
        [field](Ref<S>& self, M m) {
            S s = self.get();
            s.*(field.member) = m;
            self.set(s);
        });
}

template <typename S, typename M>
void bind_ref(py::class_<Ref<S>>& cls, const prop_info<S, M>& prop) {
    cls.def_property(
        prop.name,
        [prop](Ref<S>& self) {
            if constexpr (Structure<M>) {
                return py::cast(Ref<M>{[&self, prop]() {
                                           S s = self.get();
                                           return (s.*(prop.getter))();
                                       },
                                       [&self, prop](M value) {
                                           S s = self.get();
                                           (s.*(prop.setter))(value);
                                           self.set(s);
                                       }});
            } else if constexpr (Variant<M>) {
                S s = self.get();
                M m = (s.*(prop.getter))();
                return m.visit([prop, &self, s]<typename U>(U&& value) {
                    return py::cast(Ref<std::remove_cvref_t<U>>{[&self, prop] {
                                                                    S s_local = self.get();
                                                                    M m_local = (s_local.*(prop.getter))();
                                                                    return m_local.template get<std::remove_cvref_t<U>>();
                                                                },
                                                                [&self, prop](U new_value) {
                                                                    S s_local = self.get();
                                                                    M m_local = (s_local.*(prop.getter))();
                                                                    m_local.template get<std::remove_cvref_t<U>>() = new_value;
                                                                    (s_local.*(prop.setter))(m_local);
                                                                    self.set(s_local);
                                                                }});
                });
            }
            S s = self.get();
            return py::cast((s.*(prop.getter))());
        },
        [prop](Ref<S>& self, M m) {
            S s = self.get();
            (s.*(prop.setter))(m);
            self.set(s);
        });
}

// de.cutout.width = 2.0

template <typename T>
void register_type(py::module_& m) {
    static_assert(false, "type not registered");
}

template <typename T>
concept is_primitive = std::is_arithmetic_v<T> || std::is_same_v<T, std::string> || std::is_enum_v<T> || std::is_same_v<T, glm::dmat4x4>;

template <is_primitive T>
void register_type(py::module_& m) {};

template <Variant T>
void register_type(py::module_& m);

template <Structure T>
void register_type(py::module_& m) {
    static bool registered = false;
    if (registered) return;
    registered = true;

    const char* name = info<T>::type_name;

    std::apply(
        [&](auto&&... field) {
            (([&] {
                 using MemberType = typename std::remove_cvref_t<decltype(field)>::MemberType;
                 register_type<MemberType>(m);
             })(),
             ...);
        },
        info<T>::fields);

    py::class_<T> cls(m, name);
    cls.def(py::init<>());
    std::apply([&](auto&&... field) { (bind(cls, field), ...); }, info<T>::fields);

    // add conversion from Ref<T> to T if T is copy constructible
    if constexpr (std::is_copy_constructible_v<T>) {
        py::class_<Ref<T>> ref_cls(m, ("Ref[" + std::string(name) + "]").c_str());
        std::apply([&](auto&&... field) { (bind_ref(ref_cls, field), ...); }, info<T>::fields);

        cls.def(py::init([](const Ref<T>& ref) {
            T value = ref.get();
            return new T(value);
        }));
        py::implicitly_convertible<Ref<T>, T>();
    }
}

template <typename T, typename U>
void register_alternative(py::module_& m, py::class_<T> cls) {
    static bool registered = false;
    if (registered) return;
    registered = true;

    register_type<U>(m);

    cls.def(py::init([](const U& u) { return new T(u); }));

    py::implicitly_convertible<U, T>();
}

template <typename T, typename U, typename... Alternatives>
void register_all_alternatives(py::module_& m, py::class_<T> cls, rayx::Variant<U, Alternatives...>*) {
    (register_alternative<T, Alternatives>(m, cls), ...);
}

template <Variant T>
void register_type(py::module_& m) {
    static bool registered = false;
    if (registered) return;
    registered = true;

    const char* name = info<T>::type_name;

    py::class_<T> cls(m, name);
    cls.def(py::init<>());

    py::class_<Ref<T>> ref_cls(m, ("Ref[" + std::string(name) + "]").c_str());

    cls.def(py::init([](const Ref<T>& ref) {
        T value = ref.get();
        return new T(value);
    }));

    py::implicitly_convertible<Ref<T>, T>();

    // conversions between variant and its alternatives
    T* alt_ptr = nullptr;
    register_all_alternatives(m, cls, alt_ptr);
};

}  // namespace reflect
