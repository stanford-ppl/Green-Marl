package com.oracle.greenmarl.giraphlib;

import java.io.DataInput;
import java.io.DataOutput;
import java.io.IOException;
import java.util.Collection;
import java.util.HashSet;
import java.util.Iterator;
import java.util.Set;

import org.apache.hadoop.io.AbstractMapWritable;
import org.apache.hadoop.io.Writable;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.util.ReflectionUtils;

/**
 * A Set of IntWritable
 */
public class IntSetWritable implements Set<IntWritable>, Writable  {

  private HashSet<IntWritable> instance;
  
  /** Default constructor. */
  public IntSetWritable() {
    this.instance = new HashSet<IntWritable>();
  }
  
  /**
   * Copy constructor.
   * 
   * @param other the set to copy from
   */
  public IntSetWritable(IntSetWritable other) {
    this.instance = new HashSet<IntWritable>(other.instance);
  }
  
  /** {@inheritDoc} */
  public boolean add(IntWritable w) {
    return instance.add(w);
  }

  /** {@inheritDoc} */
  public boolean addAll(Collection<? extends IntWritable> c) {
    return instance.addAll(c);
  }
  
  /** {@inheritDoc} */
  public void clear() {
    instance.clear();
  }

  /** {@inheritDoc} */
  public boolean contains(Object o) {
    return instance.contains(o);
  }

  /** {@inheritDoc} */
  public boolean isEmpty() {
    return instance.isEmpty();
  }

  /** {@inheritDoc} */
  public boolean remove(Object o) {
    return instance.remove(o);
  }

  /** {@inheritDoc} */
  public int size() {
    return instance.size();
  }

  /** {@inheritDoc} */
  public Iterator<IntWritable> iterator() {
    return instance.iterator();
  }

  /** {@inheritDoc} */
  public Object[] toArray() {
    return instance.toArray();
  }

  /** {@inheritDoc} */
  public <T> T[] toArray(T[] a) {
    return instance.toArray(a);
  }

  /** {@inheritDoc} */
  public boolean containsAll(Collection<?> c) {
    return instance.containsAll(c);
  }

  /** {@inheritDoc} */
  public boolean retainAll(Collection<?> c) {
    return instance.retainAll(c);
  }

  public boolean retain(IntWritable o) {
    if (instance.contains(o)) {
        instance.clear();
        instance.add(o);
    }
    else {
        instance.clear();
    }
    return true;
  }

  /** {@inheritDoc} */
  public boolean removeAll(Collection<?>  c) {
    return instance.removeAll(c);
  }
  
  // Writable
  
  /** {@inheritDoc} */
  @Override
  public void write(DataOutput out) throws IOException {
    
    // Write out the number of entries in the set
    out.writeInt(instance.size());

    // Then write out each value
    for (IntWritable w : instance) {
      w.write(out);
    }
  }

  /** {@inheritDoc} */
  @SuppressWarnings("unchecked")
  @Override
  public void readFields(DataInput in) throws IOException {
    
    // First clear the set.  Otherwise we will just accumulate
    // entries every time this method is called.
    this.instance.clear();
    
    // Read the number of entries in the set
    int entries = in.readInt();
    
    // Then read each value
    
    for (int i = 0; i < entries; i++) {
      IntWritable w = new IntWritable();
      w.readFields(in);
      
      instance.add(w);
    }
  }
}

