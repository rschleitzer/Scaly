// Scaly Vector - dynamic array

export class Vector<T> {
  private data: T[]

  constructor(items?: T[] | Iterable<T>) {
    this.data = items ? [...items] : []
  }

  static fromPointer<T>(start: T[], length: number): Vector<T> {
    return new Vector(start.slice(0, length))
  }

  get length(): number {
    return this.data.length
  }

  get(index: number): T | undefined {
    return this.data[index]
  }

  set(index: number, value: T): void {
    this.data[index] = value
  }

  push(value: T): void {
    this.data.push(value)
  }

  pop(): T | undefined {
    return this.data.pop()
  }

  *[Symbol.iterator](): Iterator<T> {
    for (const item of this.data) {
      yield item
    }
  }

  toArray(): T[] {
    return [...this.data]
  }
}
