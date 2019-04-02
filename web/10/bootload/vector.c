extern void start();

extern void intr01();
extern void intr02();
extern void intr03();
extern void intr04();
extern void intr05();
extern void intr06();
extern void intr07();
extern void intr08();
extern void intr09();

extern void intr10();
extern void intr11();
extern void intr12();
extern void intr13();
extern void intr14();
extern void intr15();
extern void intr16();
extern void intr17();
extern void intr18();
extern void intr19();

extern void intr20();
extern void intr21();
extern void intr22();
extern void intr23();
extern void intr24();
extern void intr25();
extern void intr26();
extern void intr27();
extern void intr28();
extern void intr29();

extern void intr30();
extern void intr31();
extern void intr32();
extern void intr33();
extern void intr34();
extern void intr35();
extern void intr36();
extern void intr37();
extern void intr38();
extern void intr39();

extern void intr40();
extern void intr41();
extern void intr42();
extern void intr43();
extern void intr44();
extern void intr45();
extern void intr46();
extern void intr47();
extern void intr48();
extern void intr49();

extern void intr50();
extern void intr51();
extern void intr52();
extern void intr53();
extern void intr54();
extern void intr55();
extern void intr56();
extern void intr57();
extern void intr58();
extern void intr59();

extern void intr60();
extern void intr61();
extern void intr62();
extern void intr63();

void (*vectors[])() = {
  start , intr01, intr02, intr03, intr04, intr05, intr06, intr07,
  intr08, intr09, intr10, intr11, intr12, intr13, intr14, intr15,
  intr16, intr17, intr18, intr19, intr20, intr21, intr22, intr23,
  intr24, intr25, intr26, intr27, intr28, intr29, intr30, intr31,
  intr32, intr33, intr34, intr35, intr36, intr37, intr38, intr39,
  intr40, intr41, intr42, intr43, intr44, intr45, intr46, intr47,
  intr48, intr49, intr50, intr51, intr52, intr53, intr54, intr55,
  intr56, intr57, intr58, intr59, intr60, intr61, intr62, intr63,
};
