// Scaly File - wrapper around Node.js fs

import * as fs from 'node:fs'
import { ScalyString } from '@scaly/containers'

export class FileError extends Error {
  constructor(message: string) {
    super(message)
    this.name = 'FileError'
  }
}

export class File {
  static readToString(path: string): ScalyString {
    try {
      const content = fs.readFileSync(path, 'utf-8')
      return new ScalyString(content)
    } catch (e) {
      throw new FileError(`Failed to read file: ${path}`)
    }
  }

  static writeFromString(path: string, content: ScalyString | string): void {
    try {
      const str = content instanceof ScalyString ? content.toString() : content
      fs.writeFileSync(path, str, 'utf-8')
    } catch (e) {
      throw new FileError(`Failed to write file: ${path}`)
    }
  }

  static exists(path: string): boolean {
    return fs.existsSync(path)
  }

  static delete(path: string): void {
    try {
      fs.unlinkSync(path)
    } catch (e) {
      throw new FileError(`Failed to delete file: ${path}`)
    }
  }
}
