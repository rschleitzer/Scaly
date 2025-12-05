#!/usr/bin/env node
// Scaly REPL

import * as readline from 'node:readline'
import { evaluate } from '@scaly/lang'

const rl = readline.createInterface({
  input: process.stdin,
  output: process.stdout,
})

function prompt(): void {
  rl.question('scaly> ', (input) => {
    if (input.trim() === '') {
      prompt()
      return
    }

    if (input === ':q' || input === ':quit') {
      rl.close()
      return
    }

    const result = evaluate(input)

    switch (result._tag) {
      case 'Ok':
        console.log(result.value)
        break
      case 'ParseError':
        console.error(`Parse error: ${result.message}`)
        break
      case 'EvalError':
        console.error(`Eval error: ${result.message}`)
        console.error(`Generated code: ${result.code}`)
        break
    }

    prompt()
  })
}

console.log('Scaly REPL (type :q to quit)')
prompt()
