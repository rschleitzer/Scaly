#!/usr/bin/env node
// Scaly REPL

import * as readline from 'node:readline'
import { evaluate } from '@scaly/lang'

const rl = readline.createInterface({
  input: process.stdin,
  output: process.stdout,
})

function showHelp(): void {
  console.log(`Commands:
  :q, :quit     Exit REPL
  :h, :help     Show this help`)
}

function handleEval(input: string): void {
  const result = evaluate(input)
  switch (result._tag) {
    case 'Ok':
      console.log(result.value)
      break
    case 'ModelError':
      console.error(result.message)
      break
    case 'GenerateError':
      console.error(`Generate error: ${result.message}`)
      break
    case 'EvalError':
      console.error(`Eval error: ${result.message}`)
      break
  }
}

function prompt(): void {
  rl.question('scaly> ', (input) => {
    if (input.trim() === '') {
      prompt()
      return
    }

    // Commands
    if (input.startsWith(':')) {
      const cmd = input.slice(1).toLowerCase()
      switch (cmd) {
        case 'q': case 'quit':
          rl.close()
          return
        case 'h': case 'help':
          showHelp()
          break
        default:
          console.error(`Unknown command: ${input}`)
      }
      prompt()
      return
    }

    handleEval(input)
    prompt()
  })
}

console.log('Scaly REPL (type :h for help, :q to quit)')
prompt()
