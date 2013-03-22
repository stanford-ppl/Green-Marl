package com.oracle.greenmarl.giraphlib;
import org.apache.giraph.aggregators.BasicAggregator;
import org.apache.hadoop.io.Writable;
import org.apache.hadoop.io.LongWritable;

/**
 * Aggregator for aggregating a set of Writable objects by computing the union
 * The default value when nothing is aggregated is an empty set.
 */
public class LongSetUnionAggregator extends BasicAggregator<LongSetWritable> {
  @Override
  public void aggregate(LongSetWritable value) {
    getAggregatedValue().addAll(value);
  }

  public void add(LongWritable value) {
    getAggregatedValue().add(value);
  }

  @Override
  public LongSetWritable createInitialValue() {
    return new LongSetWritable();
  }
}

