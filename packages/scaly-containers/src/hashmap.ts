// Scaly HashMap - wrapper around JS Map

export class HashMap<K, V> {
  private data: Map<K, V>

  constructor(entries?: Iterable<[K, V]>) {
    this.data = new Map(entries)
  }

  get(key: K): V | undefined {
    return this.data.get(key)
  }

  set(key: K, value: V): void {
    this.data.set(key, value)
  }

  has(key: K): boolean {
    return this.data.has(key)
  }

  delete(key: K): boolean {
    return this.data.delete(key)
  }

  get size(): number {
    return this.data.size
  }

  *[Symbol.iterator](): Iterator<[K, V]> {
    yield* this.data
  }

  keys(): IterableIterator<K> {
    return this.data.keys()
  }

  values(): IterableIterator<V> {
    return this.data.values()
  }

  entries(): IterableIterator<[K, V]> {
    return this.data.entries()
  }
}
