package com.oracle.greenmarl.giraphlib;
import org.apache.giraph.aggregators.BasicAggregator;
import org.apache.hadoop.io.Writable;
import org.apache.hadoop.io.IntWritable;

/**
 * Aggregator for aggregating a set of Writable objects by computing the union
 * The default value when nothing is aggregated is an empty set.
 */
public class IntSetUnionAggregator extends BasicAggregator<IntSetWritable> {
  @Override
  public void aggregate(IntSetWritable value) {
    getAggregatedValue().addAll(value);
  }

  public void add(IntWritable value) {
    getAggregatedValue().add(value);
  }

  @Override
  public IntSetWritable createInitialValue() {
    return new IntSetWritable();
  }
}

