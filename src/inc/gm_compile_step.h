#ifndef GM_COMPILE_STEP_H
#define GM_COMPILE_STEP_H

#include "gm_ast.h"

class gm_compile_step {

protected:
        gm_compile_step() : _okay(true), _affected(false), _desc("compiler step") {}
public:
        virtual ~gm_compile_step() {}

        virtual void process(ast_procdef* p)=0; // ast_node would be ast_procdef 
        virtual const char* get_description() {return _desc;}
        virtual bool has_affected() {return _affected;}
        virtual bool is_okay() {return _okay;}

        // factory methods
        static  gm_compile_step* get_factory() {assert(false); return NULL;}
        virtual gm_compile_step* get_instance() = 0;

protected:
        virtual void set_okay(bool b) {_okay = b;}
        virtual void set_affected(bool b) {_affected = b;}          // [to be used later]
        virtual void set_description(const char* c) {_desc = c;}
private:
        bool _okay;
        bool _affected;
        const char* _desc;

};

#define GM_COMPILE_STEP(CLASS, DESC) \
class CLASS : public gm_compile_step { \
private: \
   CLASS() {set_description(DESC);}\
public: \
   virtual void process(ast_procdef*p);\
   virtual gm_compile_step* get_instance(){return new CLASS();} \
   static  gm_compile_step* get_factory(){return new CLASS();} \
};

#define GM_COMPILE_STEP_FACTORY(CLASS) CLASS::get_factory()

#endif
