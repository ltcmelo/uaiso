/+ Code from TDPL +/


interface Stat {
   void accumulate(double x);
   void postprocess();
   double result();
}
unittest {
   assert(!traits(compiles, new IncrementalStat));
}


class IncrementalStat : Stat {
   protected double _result;
   abstract void accumulate(double x);
   void postprocess() {}
   double result() {
      return _result;
   }
}


void main(){}
