#!/usr/bin/env python

import generic_parser.ILPParser as Parser
from generic_parser.Readers.mps_reader import *
from generic_parser.Readers.pisinger_reader import *
from generic_parser.Readers.fznimf_reader import FZNImfReader
from generic_parser.Readers.inc_reader import IncReader
from generic_parser.Readers.lp_reader import LPReader
from generic_parser.Writers.sugar_writer import *
from generic_parser.Writers.aspartame_writer import *
from generic_parser.Writers.casp_writer import *
from generic_parser.Writers.inc_writer import *
from os.path import exists as file_exists
import argparse
import sys

readers = {
    "mps": MPSReader,
    "pisinger": PisingerReader,
    "fznimf": FZNImfReader,
    "lp": LPReader,
    "inc": IncReader
}

writers = {
    "sugar": SugarWriter,
    "aspartame": AspartameWriter,
    "casp": CASPWriter,
    "inc": IncWriter
}

opt_strategies = ["minimize", "maximize"]


def parse_args():
    parser = argparse.ArgumentParser(
        description="This parser reads a specified file format and translates it into another one.")
    parser.add_argument("--reader", "-r", required=True, choices=readers.keys(), help="Specifies the reader to use")
    parser.add_argument("--writer", "-w", required=True, choices=writers.keys(), help="Specifies the writer to use")
    parser.add_argument("--mps-convert-float", "-f", action="store_true", dest="convert_float",
                        help="In MPS writer mode converts all float coefficients to integer")
    parser.add_argument("--out-dir", "-o", required=True, dest="outdir",
                        help="Specifies the target directory for the translated instances. stdout can be addressed "
                             "by using '-'")
    parser.add_argument("--num-instances", "-n", type=int, default=0, dest="num",
                        help="In Smallcoeff reader mode defines the number of instances read from a single file")
    parser.add_argument("--opt-strategy", "-s", dest="opt", choices=opt_strategies,
                        help="Override all specified strategies with the given one (not working yet)")
    parser.add_argument("--default-opt-strategy", "-d", dest="default_opt", choices=opt_strategies,
                        help="An optimization strategy to fallback to if no strategy is specified. "
                             "If --opt_strategy is specified this argument has no effect (not working yet)")
    parser.add_argument("--no-split", "-p", action="store_true",
                        help="Constraints of length 3 or greater aren't split into multiple constraints of length 3")
    parser.add_argument("--reader-opts", "-e", help="Pass semicolon separated key=value options to the reader")
    parser.add_argument("--writer-opts", "-i", help="Pass semicolon separated key=value options to the writer")
    parser.add_argument("input", nargs="*", metavar="input-file", default='-',
                        help="Any number of instance files to convert. Use '-' to address stdin. '-' is default")
    return parser.parse_args()


if __name__ == "__main__":
    args = parse_args()
    options = {}

    if args.no_split:
        options['no_split'] = True

    rdr_opts = {k: v for k, v in map(lambda o: o.split('='), args.reader_opts.split(';'))} if args.reader_opts else {}
    wtr_opts = {k: v for k, v in map(lambda o: o.split('='), args.writer_opts.split(';'))} if args.writer_opts else {}

    for f in args.input:
        if f == '-':
            f = sys.stdin
            sys.stderr.write("Reading from stdin...\n")
        elif not file_exists(f):
            sys.stderr.write("Error: file '%s' doesn't exist\n" % f)
            sys.exit(1)

        if args.opt:
            rdr_opts["opt_strategy"] = args.opt
        if args.default_opt:
            rdr_opts["default_opt_strategy"] = args.default_opt

        # TODO opt_strategy external set
        reader = readers[args.reader](convert_float=args.convert_float, **rdr_opts)
        writer = writers[args.writer](**wtr_opts)

        _parser = Parser.ILPParser(reader, writer, **options)

        if args.num == 0:
            args.num = float("inf")

        _parser.parse_input(f)
        _parser.write_output(args.outdir)

        if type(_parser.instance_name) == str:
            sys.stderr.write("%s parsed\n" % _parser.instance_name)
        elif f == sys.stdin:
            sys.stderr.write("<stdin> parsed\n")

        if args.reader == "pisinger" and args.num > 1:
            instances_read = 1

            while not reader.EOF and instances_read < args.num:
                offset = reader.offset
                if not _parser.parse_input(f, offset):
                    break
                _parser.write_output(args.outdir)
                instances_read += 1

                sys.stderr.write("%s parsed\n" % _parser.instance_name)
