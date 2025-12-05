// Scaly Console - standard I/O

import * as readline from 'node:readline'
import { ScalyString } from '@scaly/containers'

export class Console {
  static print(message: ScalyString | string): void {
    const str = message instanceof ScalyString ? message.toString() : message
    process.stdout.write(str)
  }

  static printLine(message: ScalyString | string = ''): void {
    const str = message instanceof ScalyString ? message.toString() : message
    console.log(str)
  }

  static async readLine(prompt: string = ''): Promise<ScalyString> {
    const rl = readline.createInterface({
      input: process.stdin,
      output: process.stdout,
    })

    return new Promise((resolve) => {
      rl.question(prompt, (answer) => {
        rl.close()
        resolve(new ScalyString(answer))
      })
    })
  }
}
