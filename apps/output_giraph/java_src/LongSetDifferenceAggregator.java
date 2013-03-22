package com.oracle.greenmarl.giraphlib;
import org.apache.giraph.aggregators.BasicAggregator;
import org.apache.hadoop.io.Writable;
import org.apache.hadoop.io.LongWritable;

/**
 * Aggregator for aggregating a set of Writable objects by computing the difference
 * The default value when nothing is aggregated is an empty set.
 */
public class LongSetDifferenceAggregator extends BasicAggregator<LongSetWritable> {
  @Override
  public void aggregate(LongSetWritable value) {
    getAggregatedValue().removeAll(value);
  }
  public void remove(LongWritable value) {
    getAggregatedValue().remove(value);
  }

  @Override
  public LongSetWritable createInitialValue() {
    return new LongSetWritable();
  }
}

