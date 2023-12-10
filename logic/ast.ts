enum ExpressionType {
	Block, /*
		{ code; code; code }
		if(a & b) { ... }
		for(1, 5) { ... }
		!!! while loops and dynamic for loops are not supported
	*/
	Variable, // type name = value;
	Statement, // function_name(args);
	Function, // function name(arg1, arg2) { function_body; }
	Return, // return value; ret value;
	Operation, // left + right
	// left & right
	// left | right
	// left => right
	// left <=> right
	// (expr)
	// ...
	Type, // bit, bit[2], bit[16]
	Number, // 0, 1, 2, 32768, -32768, -5, ...
	String // 'hello', 'world', ...
}

enum OperationType {
	'+',
	'&',
	'&&',
	'|',
	'||',
	'=>',
	'<=>',
	'()',
	'custom'
}

interface Expression {
	type: ExpressionType
}

class BlockExpression implements Expression {
	type = ExpressionType.Block
	body: Expression[]

	constructor(body) {
		this.body = body
	}
}

class VariableExpression implements Expression {
	type = ExpressionType.Variable
	name: string
	variableType: TypeExpression
	value: Expression

	constructor(name, type, value) {
		this.name = name
		this.variableType = type
		this.value = value
	}
}

class StatementExpression implements Expression {
	type = ExpressionType.Statement
	functionName: string
	arguments: BlockExpression

	constructor(name, args) {
		this.functionName = name
		this.arguments = args
	}
}

class FunctionExpression implements Expression {
	type = ExpressionType.Function
	functionName: string
	arguments: BlockExpression
	body: BlockExpression

	constructor(name, args, body) {
		this.functionName = name
		this.arguments = args
		this.body = body
	}
}

class ReturnExpression implements Expression {
	type = ExpressionType.Return
	returns: Expression

	constructor(expr) {
		this.returns = expr
	}
}

class OperationExpression implements Expression {
	type = ExpressionType.Operation
	operation: OperationType
	custom: string
}

class TypeExpression implements Expression {
	type = ExpressionType.Type
	typeName: string
	isArray: boolean
	arraySize: NumberExpression

	constructor(typeName, isArray, arraySize) {
		this.typeName = typeName
		this.isArray = isArray
		this.arraySize = arraySize
	}
}

class NumberExpression implements Expression {
	type = ExpressionType.Number
	number: boolean[]

	constructor(num) {
		this.number = num
	}
}

class StringExpression implements Expression {
	type = ExpressionType.String
	string: string

	constructor(string) {
		this.string = string
	}
}

function binaryForm(n: number): boolean[] {
	let number = n
	// convert to binary
	let bin: boolean[] = []
	for(let i = 0; i < 16; i++) {
		bin.push(number % 2 === 1)
		number = Math.floor(number / 2)
	}

	return bin
}

// types: Variable, Statement, Function, Return, Operation, Type
function parse(code: string): Expression {
	let i = 0

	function skipWhitespace() { while(' \t\n'.includes(code[i])) i++ }

	skipWhitespace()
	while (i < code.length) {
		let character = code[i]

		if (character === '{') {
			// block
		} else if (character === '\'') {
			// string
			let string = ''
			character = code[++i]

			while(character != '\'' || code[i - 1] == '\\') {
				string += character
				character = code[++i]
			}

			return new StringExpression(string.split('').map(e => binaryForm(e.charCodeAt(0))))
		}
		
		if ('0123456789'.includes(character)) {
			// number
			let number = '0123456789'.indexOf(character)
			character = code[++i]
			while('0123456789'.includes(character)) {
				number *= 10
				number += '0123456789'.indexOf(character)
				character = code[++i]
			}

			return new NumberExpression(binaryForm(number).reverse())
		}

		if(code.substring(i).startsWith('bit')) {
			i += 3
			character = code[i]
			if(character === ' ') 
		}

		skipWhitespace()
		return new NumberExpression(0)
	}

	return new NumberExpression(0)
}

const fs = require('fs')
console.log(parse(fs.readFileSync('multiplication.logic', 'utf-8')))