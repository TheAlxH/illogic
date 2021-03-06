Compilation/Installation
------------------------

The parser is written in plain Python 2.7 and doesn't require any compilation. If you want to use the [FlatZinc](http://www.minizinc.org/downloads/doc-1.6/flatzinc-spec.pdf) you have to compile the gecode based parser first. Have a look at its [README](./gecode/README) for further details.



Usage
-----

While all modules can be imported into new python scripts a command line based frontend is offered with the python `parse` script. 

```
parse --reader {READER} --writer {WRITER} 
        [--out-dir {OUTDIR}]
        [--reader-opts OPTS] [writer-opts OPTS] 
        [--restrict-lb LB] [restrict-ub UB] 
        [--split S]
        [input-file [input-file ...]
        [--help]
```

### CLI arguments

`--reader READER`
Specifies the reader (the input format) to be used. 

`--writer WRITER`
Specifies the writer (the output format) to be used.

`--out-dir OUTDIR`
(Optional) All converted files are written into `OUTDIR`. '`-`' can be used to address `stdout`. Default is '-'

`input-file [input-file ...]`
(Optional) All positional arguments are treated as input files and are processed successively. If no input file is passed the converter reads from `stdin`.

`--reader-opts OPTS` 
(Optional) Some readers allow additional configuration. Use this option to pass those information. All values are given as KEY=VALUE pairs and multiple items are separated by ';'.

`--writer-opts OPTS` 
(Optional) Some writers allow additional configuration. Use this option to pass those information. All values are given as KEY=VALUE pairs and multiple items are separated by ';'.

`--restrict-lb LB` 
(Optional) Override any open lower bound with LB. This argument is an alternative for --reader-opts='lb=LB'

`--restrict-ub UB` 
(Optional) Override any open upper bound with UB. This argument is an alternative for --reader-opts='ub=UB'

`--split S`
(Optional) If set to 1 or 2 linear integer constraints with a length greater three are split into multiple constraints. If set to 1 constraints are split with equalities. Otherwise (set to 2) they are split using inequalities. 

`--help`
(Optional) Prints a basic help for CLI usage.



Integer and Boolean Components 
------------------------------

Multiple integer and boolean components are supported by the converter:

* boolean variables (e.g. `p`, `q`)
* integer variables with open bounds (e.g. `0 ≤ x ≤ 100`, `10 ≤ x`)
* linear integer constraints (e.g. `3x + 2y ≥ 5`)
* boolean clauses (e.g. `{p, ¬q, s}`)
* an integer optimization vector (e.g. `(-2x, 4y, 7z)`)
* bool to integer constraints (e.g. `b2i(p, x) = (p ⟷ x = 1) ∧ (¬p ⟷ x == 0)`)



Currently Supported Readers and Writers
---------------------------------------

### Readers

* **mps**    [MPS format](http://lpsolve.sourceforge.net/5.5/mps-format.htm)
* **lp**     [LP format](https://www.ibm.com/support/knowledgecenter/SS9UKU_12.5.0/com.ibm.cplex.zos.help/FileFormats/topics/LP.html)
* **pisinger** [1]
* **fzn**    [Flatzinc](http://www.minizinc.org/downloads/doc-1.6/flatzinc-spec.pdf) (parser based on [gecode](http://www.gecode.org/))



| Feature \ Reader        | mps | lp  | pisinger | fzn |
|-------------------------|:---:|:---:|:--------:|:---:|
| `stdin`                 | ✓   | ✓   | ✓        | ✓   |
| gzip support            | ✓   | ✗   | ✓        | ✗   |
| discontiguous domains   | ✓   | ✓   | (✓)      | ✗   |

(✓) all domains in pisinger instances have the domain 0, 1.

_The fzn reader uses an intermediate format (fznimf) for communication between gecode and the reader. The syntax is described [here](./fznimf_syntax.txt)_



### Writers

* **Aspartame** [Potassco Aspartame](http://www.cs.uni-potsdam.de/aspartame/)
* **CASP** [Potassco clingcon](http://www.cs.uni-potsdam.de/clingcon/)
    * Use `--writer-opts="csp=DIR"` to provide an alternative csp include location. If `--writer-opts="csp="` the include statement is omitted.
* **Sugar** [sugar syntax](http://bach.istc.kobe-u.ac.jp/sugar/package/current/docs/syntax.html)


| Feature \ Writer    | aspartame | casp | sugar | inc |
|---------------------|:---------:|:----:|:-----:|:---:|
| boolean clauses     | ✓         | ✓    | ✓     | ✓   |
| bool to int         | ✓         | ✓    | ✓     | ✓   |
| reified constraints | ✗         | ✗    | ✗     | ✓   |


References
----------

[1] David Pisinger. 2005. Where are the hard knapsack problems?. Comput. Oper. Res. 32, 9 (September 2005), 2271-2284.