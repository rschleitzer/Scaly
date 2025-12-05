// Scaly Path - path manipulation utilities

import * as path from 'node:path'
import { ScalyString } from '@scaly/containers'

export class Path {
  static join(...parts: (string | ScalyString)[]): ScalyString {
    const strParts = parts.map(p => p instanceof ScalyString ? p.toString() : p)
    return new ScalyString(path.join(...strParts))
  }

  static dirname(p: string | ScalyString): ScalyString {
    const str = p instanceof ScalyString ? p.toString() : p
    return new ScalyString(path.dirname(str))
  }

  static basename(p: string | ScalyString): ScalyString {
    const str = p instanceof ScalyString ? p.toString() : p
    return new ScalyString(path.basename(str))
  }

  static extname(p: string | ScalyString): ScalyString {
    const str = p instanceof ScalyString ? p.toString() : p
    return new ScalyString(path.extname(str))
  }

  static resolve(...parts: (string | ScalyString)[]): ScalyString {
    const strParts = parts.map(p => p instanceof ScalyString ? p.toString() : p)
    return new ScalyString(path.resolve(...strParts))
  }
}
