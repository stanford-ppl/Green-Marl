package com.oracle.greenmarl.giraphlib;
import org.apache.giraph.aggregators.BasicAggregator;
import org.apache.hadoop.io.Writable;
import org.apache.hadoop.io.IntWritable;

/**
 * Aggregator for aggregating a set of Writable objects by taking intersections
 * The default value when nothing is aggregated is an empty set.
 */
public class IntSetIntersectionAggregator extends BasicAggregator<IntSetWritable> {
  @Override
  public void aggregate(IntSetWritable value) {
    getAggregatedValue().retainAll(value);
  }

  public void retain(IntWritable v) {
    getAggregatedValue().retain(v);
  }

  @Override
  public IntSetWritable createInitialValue() {
    return new IntSetWritable();
  }
}

