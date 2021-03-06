// -*- c-basic-offset: 4 -*-
#ifndef CLICK_ELEMENTMAP_HH
#define CLICK_ELEMENTMAP_HH
#include "etraits.hh"
#include <click/hashtable.hh>
class RouterT;
class ErrorHandler;

class ElementMap { public:

    ElementMap();
    ElementMap(const String&, ErrorHandler* = 0);
    ~ElementMap();

    static ElementMap* default_map()		{ return the_element_map; }
    static void push_default(ElementMap *emap);
    static void pop_default(ElementMap *emap);
    static void pop_default();

    void use()					{ _use_count++; }
    void unuse()				{ _use_count--; }

    int size() const				{ return _e.size(); }
    bool empty() const				{ return _e.size() == 1; }
    int32_t version() const			{ return _version; }

    const Traits& traits(const String&) const;
    const Traits& traits_at(int i) const	{ return _e[i]; }
    bool has_traits(const String&) const;
    int traits_index(const String&) const;

    bool provides_global(const String&) const;

    int definition_index(int i) const		{ return _e[i].def_index; }
    const String& source_directory(const Traits&) const;
    const String& package(const Traits&) const;
    const String& package(const String&) const;
    String documentation_url(const Traits&) const;

    class TraitsIterator;
    TraitsIterator begin_elements() const;

    int add(const Traits&);
    void remove_at(int);

    void parse(const String& data, ErrorHandler* = 0);
    void parse(const String& data, const String& package_name, ErrorHandler* = 0);
    void parse_xml(const String& data, const String& package_name, ErrorHandler*);
    bool parse_default_file(const String& default_path, ErrorHandler*);
    bool parse_requirement_files(RouterT*, const String& default_path, ErrorHandler*, String* not_found = 0);
    bool parse_all_files(RouterT*, const String& default_path, ErrorHandler*);
    static void report_file_not_found(String default_path, bool found_default, ErrorHandler*);
    String unparse(const String& package = String()) const;
    String unparse_nonxml() const;

    int check_completeness(const RouterT*, ErrorHandler*) const;
    bool driver_indifferent(const RouterT*, int driver_mask = Driver::ALLMASK, ErrorHandler* = 0) const;
    bool driver_compatible(const RouterT*, int driver, ErrorHandler* = 0) const;

    int provided_driver_mask() const	{ return _provided_driver_mask; }
    int driver_mask() const		{ return _driver_mask; }
    void set_driver(int d)		{ set_driver_mask(1 << d); }
    void set_driver_mask(int mask);
    int pick_driver(int wanted_driver, const RouterT* router, ErrorHandler* = 0) const;

  private:

    struct Globals {
	String srcdir;
	String compile_flags;
	String package;
	String dochref;
	int driver_mask;
    };

    Vector<Traits> _e;
    HashTable<String, int> _name_map;

    Vector<Globals> _def;

    int _use_count;
    int _driver_mask;
    int _provided_driver_mask;
    mutable int32_t _version;

    int get_driver_mask(const String&);
    int driver_elt_index(int) const;

    void collect_indexes(const RouterT*, Vector<int>&, ErrorHandler*) const;
    int compatible_driver_mask(const RouterT*, ErrorHandler* = 0) const;

    static int32_t version_counter;
    static ElementMap* the_element_map;
    void incr_version() const;

};


class ElementMap::TraitsIterator { public:

    TraitsIterator(const ElementMap*, bool elements_only);

    operator bool()			{ return _index < _emap->size(); }
    void operator++(int);

    const ElementTraits& value() const	{ return _emap->traits_at(_index); }
    int traits_index() const		{ return _index; }

  private:

    const ElementMap* _emap;
    int _index;
    bool _elements_only;

};


inline const Traits&
ElementMap::traits(const String& name) const
{
    int i = _name_map[name];
    if (!(_e[i].driver_mask & _driver_mask))
	i = driver_elt_index(i);
    return _e[i];
}

inline bool
ElementMap::has_traits(const String& name) const
{
    int i = _name_map[name];
    if (!(_e[i].driver_mask & _driver_mask) && i > 0)
	i = driver_elt_index(i);
    return i > 0;
}

inline int
ElementMap::traits_index(const String& name) const
{
    int i = _name_map[name];
    if (!(_e[i].driver_mask & _driver_mask) && i > 0)
	i = driver_elt_index(i);
    return i;
}

inline const String&
ElementMap::source_directory(const ElementTraits& t) const
{
    return _def[t.def_index].srcdir;
}

inline const String&
ElementMap::package(const ElementTraits& t) const
{
    return _def[t.def_index].package;
}

inline const String&
ElementMap::package(const String& name) const
{
    return package(traits(name));
}

inline void
ElementMap::incr_version() const
{
    _version = version_counter = (version_counter + 1) & 0x7FFFFFFF;
}

inline bool
ElementMap::provides_global(const String& req) const
{
    return _e[0].provides(req);
}

inline ElementMap::TraitsIterator
ElementMap::begin_elements() const
{
    return TraitsIterator(this, true);
}

#endif
