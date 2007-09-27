#ifndef CLICKY_GATHERERROR_HH
#define CLICKY_GATHERERROR_HH 1
#include <gtk/gtk.h>
#include <click/error.hh>
#include <click/pair.hh>
#include <click/vector.hh>

class GatherErrorHandler : public BaseErrorHandler { public:
    
    GatherErrorHandler();

    struct Message {
	int seriousness;
	String message;

	int offset1;

	int errpos1;
	int errpos2;

	Message(int s, const String &m, int off, int ep1, int ep2)
	    : seriousness(s), message(m), offset1(off), errpos1(ep1),
	      errpos2(ep2) {
	}

	int offset2() const {
	    return offset1 + message.length();
	}
    };
    
    inline int size() const {
	return _v.size();
    }

    void set_next_errpos(int ep1, int ep2) {
	_next_errpos1 = ep1;
	_next_errpos2 = ep2;
    }

    String decorate_text(Seriousness, const String &landmark, const String &text);
    void handle_text(Seriousness, const String &);
    
    void clear();
    void pop_back();
    typedef Vector<Message>::iterator iterator;
    typedef Vector<Message>::const_iterator const_iterator;
    iterator begin()			{ return _v.begin(); }
    const_iterator begin() const	{ return _v.begin(); }
    iterator end()			{ return _v.end(); }
    const_iterator end() const		{ return _v.end(); }
    iterator erase(iterator);
    iterator erase(iterator begin, iterator end);

    int begin_offset() const		{ return 0; }
    inline int end_offset() const	{ return _end_offset; }
    iterator find_offset(int offset);
    const_iterator find_offset(int offset) const;

    void calculate_offsets();
    void translate_prefix(const String &from, const String &to, int beginpos = 0);
    void run_dialog(GtkWindow *w, int beginpos = 0);

  private:
    
    Vector<Message> _v;
    int _end_offset;
    int _next_errpos1;
    int _next_errpos2;
    
};

inline void GatherErrorHandler::pop_back()
{
    assert(size());
    erase(end() - 1);
}

#endif