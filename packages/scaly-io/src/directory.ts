// Scaly Directory - directory operations

import * as fs from 'node:fs'
import { ScalyString, Vector } from '@scaly/containers'

export class Directory {
  static create(path: string | ScalyString): void {
    const str = path instanceof ScalyString ? path.toString() : path
    fs.mkdirSync(str, { recursive: true })
  }

  static exists(path: string | ScalyString): boolean {
    const str = path instanceof ScalyString ? path.toString() : path
    return fs.existsSync(str) && fs.statSync(str).isDirectory()
  }

  static list(path: string | ScalyString): Vector<ScalyString> {
    const str = path instanceof ScalyString ? path.toString() : path
    const entries = fs.readdirSync(str)
    return new Vector(entries.map(e => new ScalyString(e)))
  }

  static delete(path: string | ScalyString): void {
    const str = path instanceof ScalyString ? path.toString() : path
    fs.rmSync(str, { recursive: true })
  }
}
