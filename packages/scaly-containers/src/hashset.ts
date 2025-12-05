// Scaly HashSet - wrapper around JS Set

export class HashSet<T> {
  private data: Set<T>

  constructor(items?: Iterable<T>) {
    this.data = new Set(items)
  }

  add(value: T): void {
    this.data.add(value)
  }

  has(value: T): boolean {
    return this.data.has(value)
  }

  contains(value: T): boolean {
    return this.data.has(value)
  }

  delete(value: T): boolean {
    return this.data.delete(value)
  }

  get size(): number {
    return this.data.size
  }

  *[Symbol.iterator](): Iterator<T> {
    yield* this.data
  }
}
