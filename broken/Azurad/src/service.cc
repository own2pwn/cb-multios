/*
 * Copyright (c) 2015 Kaprica Security, Inc.
 *
 * Permission is hereby granted, cgc_free of charge, to any person obtaining a cgc_copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, cgc_copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */
#include <cstring.h>
#include "array.h"
#include "eval.h"
#include "parser.h"

const char *program = R"==END==(

proc encode_uint16(x):
    return chr(x >> 8) + chr(x)

proc encode_uint32(x):
    return chr(x >> 24) + chr(x >> 16) + chr(x >> 8) + chr(x)

proc read_uint16():
    var x
    x = ord(read(1)) << 8
    x = x | ord(read(1))
    return x

proc read_uint32():
    var x
    x = ord(read(1)) << 24
    x = x | (ord(read(1)) << 16)
    x = x | (ord(read(1)) << 8)
    x = x | (ord(read(1)))
    return x

proc main():
    echo(encode_uint32(0xdeadbeef))
    echo(encode_uint32(0x4347c000))

    var password = flag(0)
    var running = 1
    var numbers = cgc_Array()
    while running:
        var op = read_uint16()

        if op == 0xA80B:
            break
        elif op == 0x4141:
            var j = read_uint32()
            if j == password:
                var i = read_uint32()
                var f = flag(read_uint32())
                aset(numbers, i, f)
        elif op == 0xA097:
            var i = read_uint32()
            echo(encode_uint32(aget(numbers, i)))
        elif op == 0x2064:
            var i = read_uint32()
            aset(numbers, i, read_uint32())
        elif op == 0x2C56:
            var a = aget(numbers, read_uint32())
            var b = aget(numbers, read_uint32())
            aset(numbers, read_uint32(), a + b)
        elif op == 0x973D:
            var a = aget(numbers, read_uint32())
            var b = aget(numbers, read_uint32())
            aset(numbers, read_uint32(), a - b)
        elif op == 0xE925:
            var a = aget(numbers, read_uint32())
            var b = aget(numbers, read_uint32())
            aset(numbers, read_uint32(), a * b)
        elif (op == 0x71B8) || (op == 0x1510):
            var start = read_uint32()
            var cgc_end = read_uint32()
            var sum = (op == 0x1510)
            while start < cgc_end:
                var num = aget(numbers, start)
                if op == 0x71B8:
                    sum = sum + num
                else:
                    sum = sum * num
                start = start + 1
            aset(numbers, read_uint32(), sum)
        elif op == 0xC4C5:
            aset(numbers, read_uint32(), rand())
        else:
            echo(encode_uint32(op))

main()
)==END==";

static bool cgc_builtin_echo(void *arg, cgc_Evaluator &eval, const cgc_vector<cgc_unique_ptr<cgc_Var>>& args, cgc_unique_ptr<cgc_Var>& result)
{
    for (unsigned int i = 0; i < args.cgc_length(); i++)
    {
        cgc_Var *var = args[i].cgc_get();
        if (var == nullptr || var->cgc_getType() != VarType::String)
        {
            cgc_fprintf(stderr, "Error: bad type in echo\n");
            return false;
        }

        cgc_StringVar *svar = static_cast<cgc_StringVar *>(var);
        cgc_fwrite(svar->cgc_getBuffer(), svar->cgc_getLength(), stdout);
    }

    result.cgc_reset(nullptr);
    return true;
}

static bool cgc_builtin_read(void *arg, cgc_Evaluator &eval, const cgc_vector<cgc_unique_ptr<cgc_Var>>& args, cgc_unique_ptr<cgc_Var>& result)
{
    if (args.cgc_length() != 1 || !args[0] || args[0]->cgc_getType() != VarType::Number)
        return false;
    unsigned int cgc_length = static_cast<cgc_NumberVar*>(args[0].cgc_get())->cgc_getValue();
    cgc_unique_ptr<cgc_StringVar> svar = new cgc_StringVar();
    svar->cgc_resize(cgc_length);
    if (cgc_fread(svar->cgc_getBuffer(), cgc_length, stdin) != cgc_length)
        return false;
    result.cgc_reset(svar.cgc_release());
    return true;
}

static bool cgc_builtin_flag(void *arg, cgc_Evaluator &eval, const cgc_vector<cgc_unique_ptr<cgc_Var>>& args, cgc_unique_ptr<cgc_Var>& result)
{
    if (args.cgc_length() != 1 || !args[0] || args[0]->cgc_getType() != VarType::Number)
        return false;
    const unsigned int *secret_page = reinterpret_cast<const unsigned int *>(arg);
    cgc_NumberVar *var = new cgc_NumberVar(secret_page[static_cast<cgc_NumberVar*>(args[0].cgc_get())->cgc_getValue() & 0x3ff]);
    result.cgc_reset(var);
    return true;
}

static bool cgc_builtin_chr(void *arg, cgc_Evaluator &eval, const cgc_vector<cgc_unique_ptr<cgc_Var>>& args, cgc_unique_ptr<cgc_Var>& result)
{
    if (args.cgc_length() != 1 || !args[0] || args[0]->cgc_getType() != VarType::Number)
        return false;
    cgc_StringVar *svar = new cgc_StringVar();
    svar->cgc_resize(1);
    svar->cgc_getBuffer()[0] = static_cast<cgc_NumberVar*>(args[0].cgc_get())->cgc_getValue();
    result.cgc_reset(svar);
    return true;
}

static bool cgc_builtin_ord(void *arg, cgc_Evaluator &eval, const cgc_vector<cgc_unique_ptr<cgc_Var>>& args, cgc_unique_ptr<cgc_Var>& result)
{
    if (args.cgc_length() != 1 || !args[0] || args[0]->cgc_getType() != VarType::String)
        return false;
    cgc_StringVar *svar = static_cast<cgc_StringVar*>(args[0].cgc_get());
    if (svar->cgc_getLength() < 1)
        return false;
    result.cgc_reset(new cgc_NumberVar((unsigned char)svar->cgc_getBuffer()[0]));
    return true;
}

static bool cgc_builtin_rand(void *arg, cgc_Evaluator &eval, const cgc_vector<cgc_unique_ptr<cgc_Var>>& args, cgc_unique_ptr<cgc_Var>& result)
{
    static unsigned int state = 0x4347c000;
    static unsigned int idx = 0;
    const unsigned short *secret_page = reinterpret_cast<const unsigned short *>(arg);

    state ^= secret_page[idx];
    state ^= secret_page[idx+2];
    idx = (idx + 4) & 0x3ff;
    state *= 1234567;

    cgc_NumberVar *var = new cgc_NumberVar(state);
    result.cgc_reset(var);
    return true;
}

extern "C" int __attribute__((fastcall)) main(int secret_page_i, char *unused[])
{
    char *secret_page = (char *)secret_page_i;

    cgc_Parser parser(program);
    if (parser.cgc_parse())
    {
        cgc_Evaluator eval(parser.cgc_releaseRoot());
        cgc_ArrayVar::cgc_registerExternals(eval);
        eval.cgc_addExternal("chr", cgc_builtin_chr);
        eval.cgc_addExternal("echo", cgc_builtin_echo);
        eval.cgc_addExternal("ord", cgc_builtin_ord);
        eval.cgc_addExternal("flag", cgc_builtin_flag, secret_page);
        eval.cgc_addExternal("rand", cgc_builtin_rand, secret_page);
        eval.cgc_addExternal("read", cgc_builtin_read);
        if (!eval.cgc_run())
        {
            cgc_fprintf(stderr, "Eval error\n");
        }
    }
    else
    {
        cgc_fprintf(stderr, "Program error\n");
    }

    return 0;
}
