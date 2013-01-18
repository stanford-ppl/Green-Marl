#include <stdio.h>
#include <stdlib.h>
#include "gm.h"
#include "gm_mutatable_priority_map.h"
int MAX_KEY = 100;
int MAX_VAL = 100;
int gen_key() {return rand() % MAX_KEY;}
int gen_val() {return rand() % MAX_VAL;}

//int TRACE_KEY=-1;
int TRACE_KEY=35;


    /*
    for(int i=0;i<20;i++) {
        int v = rand() % 10;
        printf("map[%d] = %d\n", i, v);
        Map.setValue_seq(i, v);
    }

    Map.dump();

    for(int i=0;i<20;i++) {
        int v = rand() % 10;
        printf("map[%d] = %d\n", i, v);
        Map.setValue_seq(i, v);
    }
    printf("map[%d] = %d\n", 19,1);
    Map.setValue_seq(19,1);
    
    Map.dump();
    */

void test_main(gm_mutatable_priority_map<short, int>& Map)
{
    // key space 0 ~ 100
    // value space 0 ~ 100
    std::map<short, int> shadow;
    for(int j=0;j<1000;j++) {
        printf("\n<<<<<<<<<<< j = %d >>>>>>>>>>>>>>\n", j);

        Map.clear();
        shadow.clear();

        int last = -1;
        //-------------------------------------------------
        // test 1: insert and pop all
        //-------------------------------------------------
        printf("test1 phase1\n");
        int count = rand()%30;
        for(int i=0;i<count;i++) {
            int Key = gen_key();
            int Val = gen_val();
            shadow[Key] = Val;
            if (Key == TRACE_KEY) {printf("Before\n"); Map.dump();}
            printf("[%d] <- %d, last:%d\n", Key, Val, last);
            if (Key == TRACE_KEY) {printf("After\n"); Map.dump();printf("\n");}
            Map.setValue_seq(Key, Val);
            Map.check_integrity();
        }

        if (Map.size() != shadow.size()) {
            printf("Map:%d, shadow:%d\n", Map.size(), shadow.size());
        }

        assert(Map.size() == shadow.size());
        count = Map.size();
        for(int i=0;i<count;i++) {
           int  key = Map.getMinKey_seq();
           int  val = Map.getMinValue_seq();
           if (key == TRACE_KEY) {printf("Before\n"); Map.dump();}
           printf("key:%d, val:%d\n", key, val);
           Map.removeMinKey_seq();
           if (key == TRACE_KEY) {printf("After\n"); Map.dump();printf("\n");}
           Map.check_integrity();
           assert(shadow[key] == val);
           assert(val >= last);
           last = val;
        }

        printf("\n\ntest1 phase2\n");
        //----------------------------------------------------------
        // test 2: repeat: insert X1 elements, remove X2 elements
        //----------------------------------------------------------
        Map.clear();
        shadow.clear();
        last = -1;
        for(int j=0;j<10;j++) {
            int X1 = rand() % 20;
            int X2 = rand() % 15;

            for(int i=0;i<X1;i++) {
                int Key = gen_key();
                int Val = gen_val();
                shadow[Key] = Val;
                if (Key == TRACE_KEY) {printf("Before\n"); Map.dump();}
                Map.setValue_seq(Key, Val);
                printf("[%d] <- %d, last:%d\n", Key, Val, last);
                if (Key == TRACE_KEY) {printf("After\n"); Map.dump();printf("\n");}
                //Map.dump();
                if (Val < last) last = Val;
                Map.check_integrity();

            }

            assert(Map.size() == shadow.size());
            count = (X2 < Map.size()) ? X2 : Map.size();
            for(int i=0;i<count;i++) {
                int  key = Map.getMinKey_seq();
                int  val = Map.getMinValue_seq();
                printf("key:%d, val:%d\n", key, val);
                assert(shadow[key] == val);
                if (val < last) {
                    printf("last = %d, val = %d\n", last, val);
                }
                assert(val >= last);
                last = val;
                shadow.erase(key);
                Map.removeMinKey_seq();
                Map.check_integrity();
            }
        }

        // last output
        assert(Map.size() == shadow.size());
        count = Map.size();
        for(int i=0;i<count;i++) {
           int  key = Map.getMinKey_seq();
           int  val = Map.getMinValue_seq();
           if (key == TRACE_KEY) {printf("Before\n"); Map.dump();}
           printf("key:%d, val:%d\n", key, val);
           assert(shadow[key] == val);
           assert(val >= last);
           last = val;
           shadow.erase(key);
           Map.removeMinKey_seq();
           if (key == TRACE_KEY) {printf("After\n"); Map.dump();printf("\n");}
           Map.check_integrity();
        }
    }
}

int main()
{
    //gm_mutatable_priority_map_default_min<short, int> Map(0, 5); // default value, K
    //gm_mutatable_priority_map_simple_min<short, int> Map(0); // default value, K
    gm_mutatable_priority_map_unordered_min<short, int> Map(0); // default value, K
    test_main(Map);
}
