// Scaly String - wrapper around JS string with iterator support

export class ScalyString {
  private readonly data: string

  constructor(value: string = '') {
    this.data = value
  }

  static fromChars(chars: string[], length?: number): ScalyString {
    const len = length ?? chars.length
    return new ScalyString(chars.slice(0, len).join(''))
  }

  get length(): number {
    return this.data.length
  }

  get(index: number): string | undefined {
    return this.data[index]
  }

  equals(other: ScalyString | string): boolean {
    const otherStr = other instanceof ScalyString ? other.data : other
    return this.data === otherStr
  }

  toString(): string {
    return this.data
  }

  *[Symbol.iterator](): Iterator<string> {
    for (const char of this.data) {
      yield char
    }
  }

  getIterator(): StringIterator {
    return new StringIterator(this)
  }
}

export class StringIterator {
  private readonly str: ScalyString
  private index: number = 0

  constructor(str: ScalyString) {
    this.str = str
  }

  next(): string | null {
    if (this.index >= this.str.length) {
      return null
    }
    return this.str.get(this.index++)!
  }

  peek(): string | null {
    if (this.index >= this.str.length) {
      return null
    }
    return this.str.get(this.index)!
  }
}
