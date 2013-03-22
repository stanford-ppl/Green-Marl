package com.oracle.greenmarl.giraphlib;
import org.apache.giraph.aggregators.BasicAggregator;
import org.apache.hadoop.io.Writable;

/**
 * Aggregator for aggregating a set of Writable objects by overwriting the set
 * The default value when nothing is aggregated is an empty set.
 */
public class IntSetOverwriteAggregator extends BasicAggregator<IntSetWritable> {
  @Override
  public void aggregate(IntSetWritable value) {
    setAggregatedValue(value); // use a copy? maybe too expensive
  }

  @Override
  public IntSetWritable createInitialValue() {
    return new IntSetWritable();
  }
}

