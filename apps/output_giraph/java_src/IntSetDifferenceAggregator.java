package com.oracle.greenmarl.giraphlib;
import org.apache.giraph.aggregators.BasicAggregator;
import org.apache.hadoop.io.Writable;
import org.apache.hadoop.io.IntWritable;

/**
 * Aggregator for aggregating a set of Writable objects by computing the difference
 * The default value when nothing is aggregated is an empty set.
 */
public class IntSetDifferenceAggregator extends BasicAggregator<IntSetWritable> {
  @Override
  public void aggregate(IntSetWritable value) {
    getAggregatedValue().removeAll(value);
  }
  public void remove(IntWritable value) {
    getAggregatedValue().remove(value);
  }

  @Override
  public IntSetWritable createInitialValue() {
    return new IntSetWritable();
  }
}

