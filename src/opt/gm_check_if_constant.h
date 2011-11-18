#ifndef GM_CHECK_IF_CONST_H
#define GM_CHECK_IF_CONST_H
// todo: find a better palce to put this code

class gm_check_if_constant_t: public gm_apply
{
public:
    gm_check_if_constant_t(): _is_const(true) {set_all(false); set_for_expr(true);}
    virtual bool apply(ast_expr* e) {
        if (e->is_id() || e->is_field()) _is_const = false;  // is builtin const? --> yes
        return true;
    }
    bool is_const() {return _is_const;}
    void prepare() {_is_const = true;}
protected:
    bool _is_const;
};

#endif
