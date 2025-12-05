// Scaly List - singly linked list

class ListNode<T> {
  value: T
  next: ListNode<T> | null = null

  constructor(value: T) {
    this.value = value
  }
}

export class List<T> {
  head: ListNode<T> | null = null
  private tail: ListNode<T> | null = null
  private _length: number = 0

  get length(): number {
    return this._length
  }

  add(value: T): void {
    const node = new ListNode(value)
    if (this.tail) {
      this.tail.next = node
      this.tail = node
    } else {
      this.head = this.tail = node
    }
    this._length++
  }

  *[Symbol.iterator](): Iterator<T> {
    let current = this.head
    while (current) {
      yield current.value
      current = current.next
    }
  }

  getIterator(): ListIterator<T> {
    return new ListIterator(this)
  }
}

export class ListIterator<T> {
  private current: ListNode<T> | null

  constructor(list: List<T>) {
    this.current = list.head
  }

  next(): T | null {
    if (!this.current) return null
    const value = this.current.value
    this.current = this.current.next
    return value
  }

  hasNext(): boolean {
    return this.current !== null
  }
}
