__author__ = 'pdvalck'

# Add to static header:
# uint16_t heap = 0

from clang import cindex as ast

import util
import sys
import struct
import math

class Register:

    def __init__(self, node, index):

        self.name = node.spelling
        self.hash = node.hash
        self.index = index
        self.size = node.type.get_size()
        self.value = self.__get_value(node, 1)
        self.value = 0 if self.value == None else int(self.value, 0)

    def __str__(self):

        return '// 0x{:04x} - {} = {}'.format(self.index, self.name, self.value)

    # Recurse to get literal value (if any)
    def __get_value(self, node, mult):

        if node.kind.is_reference() and node.referenced is not None:
            node = node.referenced

        if node.kind == ast.CursorKind.MEMBER_REF_EXPR:
            node = node.get_definition()

        if node.kind == ast.CursorKind.DECL_REF_EXPR and node.get_definition() is not None:
            node = node.get_definition()

        if node.kind == ast.CursorKind.UNARY_OPERATOR:
            mult = -1

        if node.kind == ast.CursorKind.INTEGER_LITERAL:
            return str(int(node.get_tokens().next().spelling, 0) * mult)

        for c in node.get_children():
            res = self.__get_value(c, mult)
            if res != None:
                return res
        return None

    def dump(self):

        result = '// {} at address 0x{:02x} value: {}\n'.format(self.name, self.index, self.value)

        if self.value >=  0:
            if self.size == 1:
                for b in bytearray(struct.pack('>B', self.value)):
                    result += '0x' + format(b, '02x') + ','
            elif self.size == 2:
                for b in bytearray(struct.pack('>H', self.value)):
                    result += '0x' + format(b, '02x') + ','
            elif self.size == 4:
                for b in bytearray(struct.pack('>I', self.value)):
                    result += '0x' + format(b, '02x') + ','
            else:
                result += '0x00,'.join(map(lambda x: '\n' if x % 10 == 0 else '', range(self.size + 1)))
        elif self.value < 0:
            if self.size == 1:
                for b in bytearray(struct.pack('>b', self.value)):
                    result += '0x' + format(b, '02x') + ','
            elif self.size == 2:
                for b in bytearray(struct.pack('>h', self.value)):
                    result += '0x' + format(b, '02x') + ','
            elif self.size == 4:
                for b in bytearray(struct.pack('>i', self.value)):
                    result += '0x' + format(b, '02x') + ','
            else:
                result += '0x00,'.join(map(lambda x: '\n' if x % 10 == 0 else '', range(self.size + 1)))

        result += '\n\n'

        return result

class Command:

    def __init__(self, node, index, synced, ifdepth):

        self.name = node.displayname
        self.index = index
        self.size = 1 + 2
        self.hash = node.hash
        self.synced = synced
        self.ifdepth = ifdepth
        self.timing = 0

        if self.name == 'waitForTrigger' or self.name == 'check' or self.name == 'checkState':
            self.size += 2

        self.bytecode = int(util.extract_variable(node.referenced, 'bytecode'))

        timeinfo = util.extract_variable(node.referenced, 'staticExecutionTime')
        self.staticExecution = int(timeinfo if timeinfo is not None else 0)
        timeinfo = util.extract_variable(node.referenced, 'dynamicExecutionTime')
        self.dynamicExecution = int(timeinfo if timeinfo is not None else 0)
        timeinfo = util.extract_variable(node.referenced, 'staticTransitionTime')
        self.staticTransition = int(timeinfo if timeinfo is not None else 0)
        timeinfo = util.extract_variable(node.referenced, 'dynamicTransitionTime')
        self.dynamicTransition = int(timeinfo if timeinfo is not None else 0)

        self.duration = self.staticExecution + self.dynamicExecution * 128 + self.staticTransition + self.dynamicTransition * 128

        self.nextCommand = self.__extract_next_command(node.referenced)

        # Store the hash of each parameter
        self.parameters = []

        param_iter = node.referenced.get_children()

        for param in node.get_arguments():

            # Loop through the declaration as well to get the parameter names
            current_param = param_iter.next()
            while current_param.kind != ast.CursorKind.PARM_DECL:
                current_param = param_iter.next()

            if current_param.displayname == 'staticTransitionTime':
                self.duration = int(util.extract_literal(param))
            elif current_param.displayname == 'eventBitMask':
                self.parameters.append((param.type.get_size(), self.__extract_events(param)))
            else:
                self.parameters.append((param.type.get_size(), self.__extract_parameter(param)))

        for (size, hash) in self.parameters:
            self.size += size

        # Test for subcommands and process them

    def __str__(self):
        return '{} ({}) - {} - {}\n'.format(self.index, self.synced, self.size, self.name) + '\n'.join(map(str, self.parameters))

    def __extract_next_command(self, node):

        if node.kind == ast.CursorKind.VAR_DECL and node.spelling == 'nextInstruction':
            return self.__get_next_command_node(node)
        else:
            for c in node.get_children():
                res = self.__extract_next_command(c)
                if res is not None:
                    return res
            return None


    def __get_next_command_node(self, node):

        if node.kind.is_reference() and node.referenced is not None:
            node = node.referenced

        if node.kind == ast.CursorKind.MEMBER_REF_EXPR:
            node = node.get_definition()

        if node.kind == ast.CursorKind.DECL_REF_EXPR and node.get_definition() is not None:
            node = node.get_definition()

        if node.kind == ast.CursorKind.FUNCTION_DECL:
            return node

        else:
            for c in node.get_children():
                res = self.__get_next_command_node(c)
                if res is not None:
                    return res
            return None


    def __extract_parameter(self, node):

        if node.kind == ast.CursorKind.MEMBER_REF_EXPR and node.get_definition() is not None:
            node = node.get_definition()

        if node.kind == ast.CursorKind.DECL_REF_EXPR and node.get_definition() is not None:
            node = node.get_definition()

        if node.kind == ast.CursorKind.INTEGER_LITERAL:
            return node.hash
        elif node.kind == ast.CursorKind.VAR_DECL:
            return node.hash
        elif node.kind == ast.CursorKind.FIELD_DECL:
            return node.hash
        elif node.kind == ast.CursorKind.ENUM_CONSTANT_DECL:
            return node.hash
        else:
            for c in node.get_children():
                res = self.__extract_parameter(c)
                if res != None:
                    return res
            return None

    def __extract_events(self, node):


        result = 0

        if node.kind.is_reference() and node.referenced is not None:
            node = node.referenced

        if node.kind == ast.CursorKind.MEMBER_REF_EXPR:
            node = node.get_definition()

        if node.kind == ast.CursorKind.DECL_REF_EXPR and node.get_definition() is not None:
            node = node.get_definition()

        if node.kind == ast.CursorKind.ENUM_CONSTANT_DECL:
            result = int(node.get_definition().get_children().next().get_tokens().next().spelling)

        for c in node.get_children():
            result = result + int(self.__extract_events(c))

        return result

    def dump(self, registers, literals):

        result = ''

        result += '0x' + format(self.bytecode, '02x') + ','

        for b in bytearray(struct.pack('>i', self.timing)):
            result += '0x' + format(b, '02x') + ','

        for param in self.parameters:

            if param[1] in registers:
                param_value = int(registers[param[1]].index)
            elif param[1] in literals:
                param_value = int(literals[param[1]])
            else:
                param_value = param[1]

            if param[0] == 1:
                for b in bytearray(struct.pack('>b', param_value)):
                    result += '0x' + format(b, '02x') + ','
            if param[0] == 2:
                for b in bytearray(struct.pack('>h', param_value)):
                    result += '0x' + format(b, '02x') + ','
            if param[0] == 4:
                for b in bytearray(struct.pack('>i', param_value)):
                    result += '0x' + format(b, '02x') + ','

        return result

    def dump_verbose(self, registers, literals, enums):

        result = '// {} at address 0x{:02x} with timing {}\n'.format(self.name, self.index, self.timing)

        result += '0x' + format(self.bytecode, '02x') + ','

        for b in bytearray(struct.pack('>h', self.timing)):
            result += '0x' + format(b, '02x') + ','

        for param in self.parameters:

            if param[1] in registers:
                param_value = int(registers[param[1]].index)
            elif param[1] in literals:
                param_value = int(literals[param[1]])
            elif param[1] in enums:
                param_value = enums[param[1]]
            else:
                param_value = param[1]

            if param[0] == 1:
                for b in bytearray(struct.pack('>b', param_value)):
                    result += '0x' + format(b, '02x') + ','
            if param[0] == 2:
                for b in bytearray(struct.pack('>h', param_value)):
                    result += '0x' + format(b, '02x') + ','
            if param[0] == 4:
                for b in bytearray(struct.pack('>i', param_value)):
                    result += '0x' + format(b, '02x') + ','

        result += '\n'

        return result

    def add_jump_dest(self, index):
        self.parameters.append((2, index))

class Chain:

    def __init__(self, node, index=0, next=0, start_time=1):

        self.commands = []
        self.start_index = index
        self.cmd_index = index
        self.synced = False
        self.next = next
        self.start_time = start_time
        self.size = 1
        self.name = 'chain_padding'

        if node is not None:

            self.name = node.spelling
            self.__extract_commands(node, 0)
            self.size = self.cmd_index - self.start_index
            self.start_time = int(util.extract_variable(node, 'start_time'))

        if self.synced is False:
            for c in self.commands:
                c.synced = True

    def __str__(self):
        return str(self.name) + '\n' + '\n'.join(map(str,self.commands))

    def __extract_commands(self, node, ifdepth):

        if node.kind == ast.CursorKind.CALL_EXPR and node.displayname == 'sync':
            self.synced = True

        if node.kind == ast.CursorKind.IF_STMT:
            ifdepth += 1

        if node.kind == ast.CursorKind.CALL_EXPR and node.displayname != 'sync':
            #print '{} - {}'.format(self.name, node.displayname)
            self.__add_command(node, self.synced, ifdepth)

        for c in node.get_children():
            self.__extract_commands(c, ifdepth)

    def __add_command(self, node, synced, ifdepth):

        newCommand = Command(node, self.cmd_index, synced, ifdepth)
        self.cmd_index = self.cmd_index + newCommand.size
        self.commands.append(newCommand)

        while(newCommand.nextCommand is not None):
            newCommand = Command(newCommand.nextCommand, self.cmd_index, synced, ifdepth)
            self.cmd_index = self.cmd_index + newCommand.size
            self.commands.append(newCommand)

    def parse_timing(self):

        synctime = 0
        totaltime = 0

        for c in self.commands:

            if c.synced is True:
                c.timing = c.duration
            else:
                c.timing = totaltime

            if c.synced is True and synctime == 0:
                synctime = totaltime

            totaltime += int(c.duration)




        for c in self.commands:
            if c.synced is False:
                c.timing -= synctime

    def parse_ifdepth(self):

        current_ifdepth = 0

        # Loop over all commands in chain

        for i,c in enumerate(self.commands):

            # If we increase an if level (enter if clause)
            if c.ifdepth == current_ifdepth + 1:

                # Find the next command that is at lease one level less deep
                for endcmd in self.commands[i:]:
                    if endcmd.ifdepth <= current_ifdepth:
                        c.add_jump_dest(endcmd.index)
                        break

            current_ifdepth = c.ifdepth


    def dump(self, registers, literals, enums):

        result = '// Start of {}\n\n'.format(self.name)

        for c in self.commands:
            result += c.dump_verbose(registers, literals, enums)
            result += '\n'

        return result

    def dump_info(self):

        result = '// {} : 0x{:02x} -> 0x{:02x} - time: {} - next is {}\n'.format(self.name, self.start_index, self.start_index + self.size - 1, self.start_time, self.next)

        start_swap = int(struct.unpack('<H', struct.pack('>H', self.start_index))[0])
        time_swap = int(struct.unpack('<I', struct.pack('>I', self.start_time))[0])
        end_swap = int(struct.unpack('<H', struct.pack('>H', self.start_index + self.size - 1))[0])

        #result += '{{.startAddr = 0x{:04x}u, .refTimeAbs = 0x{:08x}ul, .next = 0x{:02x}u, .endAddr = 0x{:04x}u}},\n\n'.format(start_swap, time_swap, self.next, end_swap)

        # Start address
        for b in bytearray(struct.pack('>H', self.start_index)):
            result += '0x' + format(b, '02x') + ','

        # Reference time
        for b in bytearray(struct.pack('>I', self.start_time)):
            result += '0x' + format(b, '02x') + ','

        # Next
        for b in bytearray(struct.pack('>B', self.next)):
            result += '0x' + format(b, '02x') + ','

        # End address
        for b in bytearray(struct.pack('>H', self.start_index + self.size - 1)):
            result += '0x' + format(b, '02x') + ','

        result += '\n\n'

        return result

class TAISC:

    rom_variable_name = 'TAISC_ROM'
    ram_variable_name = 'ram'
    nfo_variable_name = 'TAISC_CHAINS'
    nfo_variable_type = 'ChainEntryT'

    def __init__(self, node):

        #util.pretty_node(node)

        self.literals = {}
        self.chains = {}
        self.enums = {}
        self.registers = {}

        self.rom_usage = 0
        self.rom_size = self.__get_size(node, self.rom_variable_name)
        self.rom_variable_prefix = 'const uint8_t {}[{}] = {{\n\n'.format(self.rom_variable_name, self.rom_size)
        self.rom_variable_postfix = '\n\n};\n\n'

        self.ram_usage = 0
        self.ram_size = self.__get_size(node, self.ram_variable_name)
        self.ram_variable_prefix = 'TAISC_registersT TAISC_RAM = {\n\n'
        self.ram_variable_prefix += '\t.{} = {{\n\n'.format(self.ram_variable_name)
        self.ram_variable_postfix = '\n\n\t}\n\n};\n\n'

        self.nfo_usage = 0
        self.nfo_size = self.__get_size(node, self.nfo_variable_name) / self.__get_size(node, self.nfo_variable_type)
        self.nfo_variable_prefix = 'TAISCchainsT {} = {{\n\n'.format(self.nfo_variable_name, self.nfo_size)
        self.nfo_variable_prefix += '\t.raw= {\n\n'
        self.nfo_variable_postfix = '\n\n\t}\n\n};\n\n'

        self.__extract_literals(node, 1)
        self.__extract_registers(node)
        self.__extract_chains(node)

        self.__extract_enums(node, False)

    def __get_size(self, node, name):

        node = util.find_node(node, name)

        if node is not None:
            return node.type.get_size()
        else:
            return 1

    def __extract_registers(self, node):

        self.__extract_global_registers(node, False)
        self.ram_usage = 0
        self.__extract_chain_registers(node, False)

    def __extract_chain_registers(self, node, is_valid):

        # Add a register if the node is a variable declaration
        if is_valid == True and node.kind == ast.CursorKind.VAR_DECL and node.spelling != 'start_time':
            self.__add_register(node)
        elif node.kind == ast.CursorKind.VAR_DECL and node.spelling == 'heap':
            self.__add_register(node)

        # Recurse in tree and validated when inside main
        for c in node.get_children():

            if c.kind == ast.CursorKind.FUNCTION_DECL and c.spelling.startswith('chain'):
                is_valid = True

            self.__extract_chain_registers(c, is_valid)

    def __extract_global_registers(self, node, is_valid):

        if is_valid is True and node.kind == ast.CursorKind.FIELD_DECL:
            self.__add_register(node)

        # Recurse in tree until we find RegistersT typedef
        for c in node.get_children():
            if c.kind == ast.CursorKind.TYPEDEF_DECL and c.spelling == 'TAISC_registersT':
                is_valid = True

            self.__extract_global_registers(c, is_valid)

    def __extract_literals(self, node, mult):

        # Negative number
        if node.kind == ast.CursorKind.UNARY_OPERATOR:
            mult = -1

        # Add a literal if the node is an integer literal
        if node.kind == ast.CursorKind.INTEGER_LITERAL:
            self.__add_literal(node, mult)

        # Recurse in tree but skip functions not starting with 'chain'
        for c in node.get_children():
            if not c.kind == ast.CursorKind.FUNCTION_DECL or c.spelling.startswith('chain'):
                self.__extract_literals(c, mult)

    def __extract_chains(self, node):

        # Recurse in tree and create chain for each function starting with 'chain'
        for c in node.get_children():
            if c.kind == ast.CursorKind.FUNCTION_DECL and c.spelling.startswith('chain'):
                self.__add_chain(c)
            self.__extract_chains(c)

    def __extract_enums(self, node, valid):

        if valid is True and node.kind == ast.CursorKind.ENUM_CONSTANT_DECL:
            value = int(node.get_children().next().get_tokens().next().spelling, 0)

            #print '{} {}'.format(node.spelling, value),
            #for b in bytearray(struct.pack('>i', value)):
            #    print '0x{:02x},'.format(b),
            #print ''
            self.enums[node.hash] = value


        for c in node.get_children():

            if c.kind == ast.CursorKind.ENUM_DECL:
                valid = True
            self.__extract_enums(c, valid)

    def __add_register(self, node):

        newRegister = Register(node, self.ram_usage)
        self.ram_usage = self.ram_usage + newRegister.size
        self.registers[newRegister.hash] = newRegister

    def __add_literal(self, node, mult):
        self.literals[node.hash] = int(node.get_tokens().next().spelling, 0) * mult

    def __add_chain(self, node):

        newChain = Chain(node, index=self.rom_usage)
        self.chains[node.hash] = newChain
        self.rom_usage += newChain.size
        self.nfo_usage += 1

    def __byte_padding(self, size, usage):

        padding = '//Padding from {} to {}\n'.format(usage, size - 1)
        padding += '0x00,'.join(map(lambda x: '\n' if x % 10 == 0 else '', range(size - usage + 1)))
        padding += '\n\n'

        return padding

    def __chain_padding(self, size, usage):

        emptyChain = Chain(None, self.rom_usage, next=self.nfo_size, start_time=0)

        result = ''

        for i in range(size - usage):
            result += emptyChain.dump_info()

        return result

    def __add_tab(self, string):
        return '\t' + string.replace('\n', '\n\t')

    def dump(self):

        dump_string = '#include \"../util/taisc-typedef.h\"\n\n'

        # RAM

        tempregs = {}
        maxram = 0
        minram = 0

        for reg in self.registers:
            if self.registers[reg].name != 'ram':
                tempregs[self.registers[reg].index] = self.registers[reg]
            else:
                minram = self.registers[reg].index
                maxram = minram + self.registers[reg].size

        ram_data = ''

        for reg in sorted(tempregs):
            if minram <= reg and reg < maxram:
                ram_data += tempregs[reg].dump()

        #ram_data += self.__byte_padding(self.ram_size, self.ram_usage)

        ram_data = self.__add_tab(ram_data)
        ram_data = self.__add_tab(ram_data)
        ram_data = ram_data[:-7]

        # ROM

        tempchains_index = {}
        tempchains_time = {}

        for c in self.chains:

            self.chains[c].parse_timing()
            self.chains[c].parse_ifdepth()
            tempchains_index[self.chains[c].start_index] = self.chains[c]
            tempchains_time[self.chains[c].start_time] = self.chains[c]

        rom_data = ''

        for c in sorted(tempchains_index):
            rom_data += str(tempchains_index[c].dump(self.registers, self.literals, self.enums))


        #rom_data += self.__byte_padding(self.rom_size, self.rom_usage)
        rom_data = self.__add_tab(rom_data)
        rom_data = rom_data[:-5]

        # Chain info

        nfo_data = ''

        next_chain = 1
        for c in sorted(tempchains_time):
            tempchains_time[c].next = next_chain
            next_chain += 1

        if len(tempchains_time) > 0:
            tempchains_time[max(tempchains_time.keys())].next = self.nfo_size

        for c in sorted(tempchains_time):
            nfo_data += tempchains_time[c].dump_info()


        nfo_data += self.__chain_padding(self.nfo_size, self.nfo_usage)
        nfo_data = nfo_data[:-3]

        nfo_data = self.__add_tab(self.__add_tab(nfo_data))

        dump_string += self.ram_variable_prefix
        dump_string += ram_data
        dump_string += self.ram_variable_postfix

        dump_string += self.rom_variable_prefix
        dump_string += rom_data
        dump_string += self.rom_variable_postfix

        dump_string += self.nfo_variable_prefix
        dump_string += nfo_data
        dump_string += self.nfo_variable_postfix

        return dump_string

import os.path

if os.path.isfile('/usr/lib/x86_64-linux-gnu/libclang.so.1'):
    ast.Config.set_library_file('/usr/lib/x86_64-linux-gnu/libclang.so.1')
    
index = ast.Index.create()

translation_unit = index.parse(sys.argv[1], ['-fshort-enums', '-D TAISC_COMPILER_ACCESS', '-D REMOVE_ATTR_PACKED'])

util.print_diag(translation_unit)

processor = TAISC(translation_unit.cursor)

if len(sys.argv) > 2:
    f = open(sys.argv[2], 'w')
    f.write(processor.dump())
    f.close()
else:
    print processor.dump()

