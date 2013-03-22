package com.oracle.greenmarl.giraphlib;
import org.apache.giraph.aggregators.BasicAggregator;
import org.apache.hadoop.io.Writable;
import org.apache.hadoop.io.LongWritable;

/**
 * Aggregator for aggregating a set of Writable objects by taking intersections
 * The default value when nothing is aggregated is an empty set.
 */
public class LongSetIntersectionAggregator extends BasicAggregator<LongSetWritable> {
  @Override
  public void aggregate(LongSetWritable value) {
    getAggregatedValue().retainAll(value);
  }

  public void retain(LongWritable v) {
    getAggregatedValue().retain(v);
  }

  @Override
  public LongSetWritable createInitialValue() {
    return new LongSetWritable();
  }
}

