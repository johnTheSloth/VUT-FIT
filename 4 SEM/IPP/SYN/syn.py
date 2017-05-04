# IPP - Projekt 2
# SYN: Zvýraznění syntaxe
# xgross09, Jan Grossmann

import re, sys, os, argparse, operator

# class that represents syntax highlighter
class syn(object):

    def __init__(self):
        self.files = {'output':None, 'input':None, 'format':None}
        self.format = []
        self.br = False

    # basic low level arguments check
    def basicArgsCheck(self):
        flags = {'i':True, 'o':True, 'b':True, 'f':True, 'h':True}
        for arg in sys.argv[1:]:
            if '-i' in arg and flags['i']:
                if '=' not in arg:
                    self.throwErr('Arg without value.',1)
                flags['i'] = False
            elif '-o' in arg and flags['o']:
                if '=' not in arg:
                    self.throwErr('Arg without value.',1)
                flags['o'] = False
            elif '-b' in arg and flags['b']:
                flags['b'] = False
            elif '-f' in arg and flags['f']:
                if '=' not in arg:
                    self.throwErr('Arg without value.',1)
                flags['f'] = False
            elif '-h' in arg and flags['h']:
                flags['h'] = False
            else:
                self.throwErr('Multiple args.',1)

    # Error function
    def throwErr(self, text, exitcode):
        print(text,file=sys.stderr)
        sys.exit(exitcode)

    # Find if files or dirs are missing
    def checkFiles(self):
        for k,v in self.files.items():
            if(k != 'output' and v != None and v != sys.stdin and v!= sys.stdout and not os.path.exists(v)):
                if k != 'format':
                    self.throwErr('File doesn\'t exist',2)
                    pass;
        if self.files['output'] != None:
            if self.files['output'] != sys.stdout:
                dirre = re.search('(.*)/.+', self.files['output'])
                if os.path.exists(self.files['output']) and not os.access(self.files['output'], os.W_OK):
                    self.throwErr('File is read-only.',3)
                elif dirre is not None and not os.path.exists(dirre.group(1)):
                    self.throwErr('Directory doesn\'t exist',3)

    # Main arg paresr
    def parseArgs(self, argv = sys.argv):
        parser = argparse.ArgumentParser(add_help=False)
        parser.add_argument('-i','--input', nargs='?')
        parser.add_argument('-o','--output', nargs='?')
        parser.add_argument('-f','--format', nargs='?')
        parser.add_argument('-b','--br', action='store_true', default=False)
        parser.add_argument('-h','--help', action='store_true', default=False)

        try:
            args = parser.parse_args()
        except:
            self.throwErr('Invalid arguments.',1)

        # If input argument is invalid
        if args.input is not None and  len(args.input) == 0:
            self.throwErr('Invalid input',1)
        # set input to STDIN
        if args.input == None:
            args.input = sys.stdin
        self.files['input'] = args.input

        # set br flag
        if args.br == True:
            self.br = True

        # If output is not set, set output as STDOUT
        if args.output == None:
            args.output = sys.stdout
        self.files['output'] = args.output

        # Set format file
        self.files['format'] = args.format

        # print help message
        if args.help:
            if len(argv) == 2:
                print("┏━━━━━━━━━━┓")
                print("┃ NÁPOVĚDA ┃")
                print("┗┳━━━━━━━━━┛")
                print(" ┣━ SYN: Zvýraznění syntaxe")
                print(" ┣━ Skript provádí automatické zvýrazňování různých částí textu.")
                print(" ┗━ Skript pracuje s tabulkou regulárních výrazů a výstupním formátováním.")
                print("")
                print("┏━━━━━━━━━━━┓")
                print("┃ ARGUMENTY ┃")
                print("┗┳━━━━━━━━━━┛")
                print(" ┣━ --help            : Zobrazení nápovědy")
                print(" ┣━ --input=filename  : Zadaný vstupní soubor - Defaultně stdin")
                print(" ┣━ --format=filename : Zadaný formátovací soubor")
                print(" ┣━ --output=filename : Zadaný výstupní soubor - Defaultně stdout")
                print(" ┗━ --br              : Přidá element <br /> na konec každého řádku")
                exit(0)
            else:
                # Or throw error message.
                self.throwErr('Invalid help and args combination.',1)

    # Function to read and parse format file into data structures for further usage
    def parseFormatFile(self):
        f = [line.rstrip() for line in open(self.files['format'])]
        # Skip blank lines
        lines = (line for line in f if line)
        for line in lines:
            # Check if format of format file is valid
            m = re.search("(.+?)\t+(.+)", line)
            if m is None:
                self.throwErr('Invalid format file.',4)
            if len(m.groups()) is not 2:
                self.throwErr('Invalid format file.',4)
            for char in m.group(1):
                if ord(char) < 32:
                    self.throwErr('Invalid Char in format',4)
            # Edit regexes
            key =  m.group(1).strip()
            # Escape orig. regexes
            if (re.match(r"[^%]{0,1}\.{2,}", key) != None):
                self.throwErr('Bad format file.',4)
            if (re.search('\\\\',key) is not None):
                key = re.sub(r'\\\\',r'\\\\\\\\',key)
            # Get rid of invalid character combinations
            if (re.search('\.\*|^\(\)$|^\(\)|[^%]\|$|[^%]!$|[^%]\(\)|^!$|^\|$|^\||[^%]\|$|^\*|^\+|!\.|[^%]\|\||\.\.|^\.|[^%]\.$|[^%]\.\||\|\.|%b|%c|%e|%f|%g|%h|%i|%j|%k|%m|%o|%p|%q|%r|%u|%v|%x|%y|%z|%A|%B|%C|%E|%F|%G|%H|%I|%J|%K|%M|%O|%P|%Q|%R|%U|%V|%X|%Y|%Z',key) !=None):
                self.throwErr("Invalid format. bum",4)
            if (re.search('[^%]\(',key)!=None):
                if (re.search('\(.+?\)',key) ==None):
                    self.throwErr("Invalid format.",4)

            # Check validity of brackets
            count = 0
            ps = True
            for i in key:
                if i == '%':
                    ps = False
                elif ps and i == "(":
                    count += 1
                    ps = True
                elif ps and i == ")":
                    count -= 1
                    ps = True
                else:
                    ps = True
            if count != 0:
                # Brackets are not in pairs
                self.throwErr("Invalid format.",4)

            # Check for double negation
            if (re.search('!.*!',key)!=None):
                if (re.search('[a-zA-Z0-9]!![a-zA-Z0-9]',key) !=None):
                    self.throwErr("Invalid format.",4)
            elif (re.search('%!',key)!=None):
                pass
            elif (re.search('!([\||\{|\}]|^\.|\*|\?|\+)',key)!=None): # another negation check
                self.throwErr("Invalid format.",4)

        	# apply NQS
            while re.search('(\+\++)+', key) != None:
                key = re.sub('(\+\++)+', '+', key);
            while re.search('(\*\*+)+', key) != None:
                key = re.sub('(\*\*+)+', '*', key);
            while re.search('(\++\*+)+', key) != None:
                key = re.sub('(\++\*+)+', '*', key);
            while re.search('(\*+\++)+', key) != None:
                key = re.sub('(\*+\++)+', '*', key);

            # change every format regex to python regex
            key = key.replace("\\n","\\\\n")
            key = key.replace("\t","\\\\t")
            key = key.replace("\\w","\\\\w")
            key = key.replace("\\W","\\\\W")
            key = key.replace("\\s","\\\\s")
            key = key.replace("\\S","\\\\S")
            key = key.replace("\\d","\\\\d")
            key = key.replace("\\D","\\\\D")
            key = key.replace("\\t","\\\\t")
            key = key.replace("?","\\?")
            key = key.replace("}","\\}")
            key = key.replace("{","\\{")
            key = key.replace("^","\\^")
            key = key.replace("]","\\]")
            key = key.replace("[","\\[")
            key = key.replace("$","\\$")
            # remove multiple %
            key = key.replace("%%", "%")
            key = key.replace("%!","!")
            # sub needed because of capture group
            key = re.sub(r"([^%])\.","\g<1>",key)
            key = key.replace("!%a", "")
            key = key.replace("%a", "[\w\W]")
            # negation
            # sub needed because of capture group
            key = re.sub(r"!(%{0,1}[^%\+\*!\(\)])","[^\g<1>]",key)
            key = re.sub(r"!(\(.*\))","[^\g<1>]",key)

            key = key.replace("%s","\s")
            key = key.replace("%d","\d")
            key = key.replace("%l","[a-z]")
            key = key.replace("%L","[A-Z]")
            key = key.replace("%w","[a-zA-Z]")
            key = key.replace("%W","[a-zA-Z0-9]")
            key = key.replace("%t","\t")
            key = key.replace("%n","\n")
            key = key.replace("%.","\.")
            key = key.replace("%|","\|")
            key = key.replace("%*","\*")
            key = key.replace("%+","\+")
            key = key.replace("%(","\(")
            key = key.replace("%)","\)")

            # Edit tags
            addTags = []
            tags = [x.strip() for x in m.group(2).split(',')]
            size = re.match('size:([1-7])',str(m.group(2)))
            color = re.match('color:([a-fA-F0-9]{6})',str(m.group(2)))

            # Find size and color but add rules in right order
            # double check for size and color is needed
            if size is not None:
                fTag = '<font size='+size.group(1);
                if color is not None:
                    fTag = fTag+' color=#'+color.group(1)
                fTag = fTag+'>'
                addTags = addTags + [fTag,'</font>']
            elif color is not None:
                fTag = '<font color=#'+color.group(1)
                fTag = fTag+'>'
                addTags = addTags + [fTag,'</font>']
            # Add tags that were easy to find
            for tag in tags:
                if tag == 'bold':
                    addTags = addTags +['<b>','</b>']
                if tag == 'italic':
                    addTags = addTags +['<i>','</i>']
                if tag == 'underline':
                    addTags = addTags +['<u>','</u>']
                if tag == 'teletype':
                    addTags = addTags +['<tt>','</tt>']
                if tag != 'italic' and tag != 'bold' and tag != 'teletype' and tag != 'underline' and 'color' not in tag and 'size' not in tag:
                    # Non-matching tag, throw error
                    self.throwErr('Invalid tag.',4)
            if size is not None:
                size = None
            else:
                size = re.search('size\s*:\s*([1-7])',str(m.group(2)))
            if color is not None:
                color = None
            else: # and another check for color and size
                color = re.search('color\s*:\s*([a-fA-F0-9]{6})',str(m.group(2)))
            if size is not None:
                fTag = '<font size='+size.group(1);
                if color is not None:
                    fTag = fTag+' color=#'+color.group(1)
                fTag = fTag+'>'
                addTags = addTags + [fTag,'</font>']
            elif color is not None:
                fTag = '<font color=#'+color.group(1)
                fTag = fTag+'>'
                addTags = addTags + [fTag,'</font>']

            color = re.search('color\s*:\s*([a-fA-F0-9]{6})',str(m.group(2)))
            if ('color' in str(m.group(2))):
                if color is None:
                    self.throwErr("Invalid color.",4)
            size = re.search('size\s*:\s*([1-7])',str(m.group(2)))
            if ('size' in str(m.group(2))):
                if size is None:
                    self.throwErr("Invalid size.",4)

            # Add to rules definition
            self.format.append([key,addTags])


    def highlight(self):
        # Open output and input files
        if self.files['output'] != sys.stdout:
            fo = open(self.files['output'],'w')

        if self.files['input'] != sys.stdin:
            fin = open(self.files['input'])
        else:
            fin = sys.stdin

        # Read input file into list
        fi = ''.join([line for line in fin])
        matches = {}
        # Loop through every rule in self.format
        for x in self.format:
            # To remove duplicities in findall
            pattern = re.compile(''+str(x[0]))

            for m in pattern.finditer(fi):
                if m.group(0) != "":
                    # This will find position where to append opening or closing tags in right order
                    if not m.start() in matches:
                        matches[m.start()] = ''.join([val for val in x[1][::2]])
                    else:
                        matches[m.start()] =  matches[m.start()] + ''.join([val for val in x[1][::2]])
                    if not m.end() in matches:
                        matches[m.end()] = ''.join(list(reversed([val for val in x[1][1::2]])))
                    else:
                        matches[m.end()] = ''.join(list(reversed([val for val in x[1][1::2]]))) + matches[m.end()]

        # Sort by position indexes
        sorted_matches = sorted(matches.items(), key=operator.itemgetter(0))

        add = 0
        # Insert all of tags to position marked in matches
        for k,v in sorted_matches:
            fi = fi[:k+add] + v + fi[k+add:]
            # Change position of indexes because of added characters
            add = add + len(v)

        # Add br to end of lines if needed
        if self.br == True:
            fi = re.sub('\n','<br />\n',fi)

        # Write output to file
        if self.files['output'] != sys.stdout:
            fo.write(fi)
            fo.close()
        else:
            # Or print it to stdout
            print(fi, end = '')

    # Core function
    def execute(self):
        self.basicArgsCheck()
        self.parseArgs()
        self.checkFiles()

        # If format file is not found, output = input
        if self.files['format'] is not None and os.path.exists(self.files['format']):
            self.parseFormatFile()
        self.highlight()


if __name__ == '__main__':
    # Create new object of syn()
    syntax = syn()
    syntax.execute()
    sys.exit(0)
