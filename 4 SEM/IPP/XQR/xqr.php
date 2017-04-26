<?php
# IPP Projekt 1: XML Query
# Autor: Jan Grossmann - xgross09@stud.fit.vutbr.cz

class Xqr {

        //FLAGs
        public $flags  = array(
            "hf" => false,
            "if" => false,
            "of" => false,
            "qf" => false,
            "qff" => false,
            "nf" => false,
            "rf" => false
        );

        //program ARGS
        public $params = array(
            'inputFile' => "",
            'outputFile' => "",
            'queryFile' => "",
            'query' => "",
            'rootElem' => ""
        );

        //parsed QUERY
        public $queryElems = array(
            'root' => "ROOT",
            'select' => "",
            'from' => "",
            'whSet' => false,
            'where' => array('not' => false),
            'orderBy' => "",
            'limit' => "unlimited",
        );

        //VARs
        public $query = "";
        public $queryArr = array();
        public $xmlDom;
        public $xml;


        function debugText()
        {
            echo "Query: " . $this->query . "\n";
            print_r($this->queryArr);
            print_r($this->queryElems);
        }

        function help()
        {
            echo "┏━━━━━━━━━━┓\n";
            echo "┃ NÁPOVĚDA ┃\n";
            echo "┗┳━━━━━━━━━┛\n";
            echo " ┣━ XQR: XML Query\n";
            echo " ┣━ Skript provádí vyhodnocení zadaného dotazu nad vstupem ve formátu XML.\n";
            echo " ┗━ Výstupem je XML obsahující elementy splňující požadavky dané dotazem.\n";
            echo "\n";
            echo "┏━━━━━━━━━━━┓\n";
            echo "┃ ARGUMENTY ┃\n";
            echo "┗┳━━━━━━━━━━┛\n";
            echo " ┣━  -n               : Negenerovat XML hlavičku na výstup skriptu\n";
            echo " ┣━ --help            : Zobrazení nápovědy\n";
            echo " ┣━ --input=filename  : Zadaný vstupní soubor ve formátu XML\n";
            echo " ┣━ --output=filename : Zadaný výstupní soubor ve formátu XML\n";
            echo " ┣━ --query='dotaz'   : zadaný dotaz v dotazovacím jazyce (bez apostrofu) \n";
            echo " ┣━ --qf=filename     : Dotaz v dotazovacím jazyce, zadaný v externím\n";
            echo " ┃                    : textovém souboru (nelze kombinovat s --query)\n";
            echo " ┗━ --root=element    : Jméno párového kořenového elementu obalující výsledky\n";

            exit(0);
        }

        function checkFlags($flagName)
        {

            if ($this->flags[$flagName]) {
                fwrite(STDERR, "ERR: Argument zadan dvakrat.\n");
                exit(1);
            } else {
                $this->flags[$flagName] = true;
            }
            if ($flagName != "hf" && $this->flags["hf"]) {
                fwrite(STDERR, "ERR: Help v kombinaci s jinym argumentem.\n");
                exit(1);
            }
            if ($this->flags["qf"] && $this->flags["qff"]) {
                fwrite(STDERR, "ERR: Argumenty --qf a --query spolecne.\n");
                exit(1);
            }

            foreach ($this->flags as $key => $value) {                      // help is last arg
                if ($key != "hf" && $value && $this->flags["hf"]) {
                    fwrite(STDERR, "ERR: Help v kombinaci s jinym argumentem.\n");
                    exit(1);
                }
            }
        }

        // Open and read query file or load query into desired variable
        function getQuery()
        {
            if ($this->flags["qff"]) {
                if (file_exists($this->params["queryFile"]) && is_readable($this->params["queryFile"]) && filesize($this->params["queryFile"])) {
                    $qfile = fopen($this->params["queryFile"], "r");
                    $this->query = fread($qfile, filesize($this->params["queryFile"]));
                    fclose($qfile);
                } else {
                    fwrite(STDERR, "ERR: Query soubor neni validni.\n");
                    exit(4);
                }
                $this->query = trim(preg_replace('/\s+/', ' ', $this->query));
            } else if ($this->flags["qf"] && $this->params["query"] != "") {
                $this->query = trim(preg_replace('/\s+/', ' ', $this->params["query"]));
            } else {
                fwrite(STDERR, "ERR: Query chybi?!?");
                exit(4);
            }
        }

        // Function to validate and parse query into array $queryElems
        function validateQuery(&$queryCp)
        {
            // SELECT
            if (preg_match("/^SELECT\b/", $queryCp, $match)) {
                $this->queryAppend($match[0],$queryCp);

            } else {
                fwrite(STDERR, "ERR: Invalidni query dotaz.\n");
                exit(80);
            }

            // SELECT element
            if (preg_match("/^[a-zA-Z]+/", $queryCp, $match)) {
                $this->queryAppend($match[0],$queryCp);

                if (strpos("SELECTFROMLIMITWHERENOT", $match[0]) !== false) {
                    fwrite(STDERR, "ERR: Chybejici element.\n");
                    exit(80);
                }
                // add ELEMENT to array
                $this->queryElems["select"] = $match[0];
            } else {
                fwrite(STDERR, "ERR: Invalidni query dotaz. (element)\n");
                exit(80);
            }

            // SELECT element FROM
            if (preg_match("/^FROM\b/", $queryCp, $match)) {
                $this->queryAppend($match[0],$queryCp);
            } else {
                fwrite(STDERR, "ERR: Invalidni query dotaz. (FROM)\n");
                exit(80);
            }

            // SELECT element FROM <FROM-ELM>
            if (preg_match("/^ROOT\b/", $queryCp, $match)) {    // ROOT
                $this->queryAppend($match[0],$queryCp);
                //add ELEMENT to array
                $this->queryElems["from"] = "ROOT";
            } else {
                //add ELEMENT to array
                $this->queryElems["from"] = $this->getElOrAt($queryCp);
            }

            // SELECT element FROM el WHERE
            if (preg_match("/^WHERE\b/", $queryCp, $match)) {   // WHERE
                $this->queryAppend($match[0],$queryCp);
                $this->queryElems["whSet"] = true;
                // <WHERE-CLAUSE> <CONDITION>
                $this->getCondition($queryCp);
            } else {
                // WHERE missing
            }

            if (preg_match("/^ORDER BY\b/", $queryCp, $match)) {            // ORDER BY
                $this->queryAppend($match[0],$queryCp);
                $this->queryElems['orderBy']['el'] = $this->getElOrAt($queryCp);
                if (preg_match("/^ASC\b/", $queryCp, $match)) {             // ASC
                    $this->queryAppend($match[0],$queryCp);
                    $this->queryElems['orderBy']['way'] = "ASC";
                } else if (preg_match("/^DESC\b/", $queryCp, $match)) {     // DESC
                    $this->queryAppend($match[0],$queryCp);
                    $this->queryElems['orderBy']['way'] = "DESC";
                } else {
                    fwrite(STDERR, "ERR: Invalidni query dotaz. (ORDERING)\n");
                    exit(80);
                }
            }

            $this->getLimit($queryCp);
            if ($queryCp != "") {
                fwrite(STDERR, "ERR: Invalidni query dotaz.\n");
                exit(80);
            }
        }

        // Add query to $queryArr
        function queryAppend($x, &$queryCp)
        {
            $this->queryArr[] = $x;
            $queryCp = trim(substr($queryCp, strlen($x)));

        }

        // Find element, attribute or both in query string
        function getElOrAt(&$queryCp)
        {
            $ret = "";
            if (preg_match("/^\.[a-zA-Z]+/", $queryCp, $match)) {                   // <CONDITION> .at
                $this->queryAppend($match[0],$queryCp);
                $ret["el"] = "";
                $ret["at"] = ltrim($match[0], '.');
            } else if (preg_match("/^[a-zA-Z]+\.[a-zA-Z]+/", $queryCp, $match)) {   //	<CONDITION> el.at
                $this->queryAppend($match[0],$queryCp);
                $ret["el"] = explode(".", $match[0])[0];
                $ret["at"] = explode(".", $match[0])[1];
            } else if (preg_match("/^[a-zA-Z]+/", $queryCp, $match)) {              //	<CONDITION> el
                if ($match[0] != "LIMIT") {
                    $this->queryAppend($match[0],$queryCp); // check if there's missing element at "SELECT el FROM <HERE> LIMIT"
                    $ret["el"] = $match[0];
                    $ret["at"] = "";
                } else {
                    return;
                }
            }

            // check for keywords
            if (!empty($match) && strpos("SELECTFROMLIMITWHERENOTORDER", $match[0]) !== false) {
                fwrite(STDERR, "ERR: Chybejici element nebo atribut.\n");
                exit(80);
            }

            return $ret;

        }

        // Find limit from query and store it
        function getLimit(&$queryCp)
        {
            if (preg_match("/^LIMIT\b/", $queryCp, $match)) {               // LIMIT
                $this->queryAppend($match[0],$queryCp);
                //echo $queryCp;
                if (preg_match("/[\+|\-]?[0-9]+/", $queryCp, $match)) {     //number
                    $this->queryAppend($match[0],$queryCp);
                    if ((float) $match[0] < 0) {
                        fwrite(STDERR, "ERR: Zaporna hodnota pro LIMIT.\n");
                        exit(80);
                    }
                    $this->queryElems['limit'] = $match[0];
                } else {
                    fwrite(STDERR, "ERR: Chybejici hodnota pro LIMIT.\n");
                    exit(80);
                }
            }
        }

        // Find next relational operator in query
        function getRelOp(&$queryCp)
        {
            if (preg_match("/^CONTAINS\b/", $queryCp, $match)) {    // CONTAINS
                $this->queryAppend($match[0],$queryCp);
            } else if (preg_match("/^=/", $queryCp, $match)) {      // =
                $this->queryAppend($match[0],$queryCp);
            } else if (preg_match("/^>/", $queryCp, $match)) {      // >
                $this->queryAppend($match[0],$queryCp);
            } else if (preg_match("/^</", $queryCp, $match)) {      // <
                $this->queryAppend($match[0],$queryCp);
            } else {
                fwrite(STDERR, "ERR: Chybejici RelOp.\n");
                exit(80);
            }
            return $match[0];
        }

        // Find next literal in query
        function getLiteral(&$queryCp)
        {
            $ret = array();
            if (preg_match("/^\".+?\"/", $queryCp, $match)) {       // string ''
                $this->queryAppend($match[0],$queryCp);
                $ret["type"] = "str";
                $ret["val"] = substr($match[0],1,-1);
            }else if (preg_match("/^\'.+?\'/", $queryCp, $match)) { // string ""
                $this->queryAppend($match[0],$queryCp);
                $ret["type"] = "str";
                $ret["val"] = substr($match[0],1,-1);
            } else if (preg_match("/^[\+|\-]?[0-9]+\.?[0-9]*/", $queryCp, $match)) { //number
                $this->queryAppend($match[0],$queryCp);
                $ret["type"] = "num";
                $ret["val"] = (float)$match[0];
            } else {
                fwrite(STDERR, "ERR: Invalidni query dotaz.\n");
                exit(80);
            }
            return $ret;
        }

        // Find condition part in query
        function getCondition(&$queryCp)
        {
            if (preg_match("/^NOT\b/", $queryCp, $match)) {          // NOT <CONDITION>
                $this->queryAppend($match[0],$queryCp);
                $this->getCondition($queryCp);                       // repeat until there's an element
                $this->queryElems["where"]["not"] = !$this->queryElems["where"]["not"];
            } else {
                $this->queryElems["where"]["el"] = $this->getElOrAt($queryCp);      // get element
                $this->queryElems["where"]["relOp"] = $this->getRelOp($queryCp);    // get rel. op.
                $this->queryElems["where"]["lit"] = $this->getLiteral($queryCp);    // get literal
                if($this->queryElems["where"]["relOp"] == "CONTAINS" && $this->queryElems["where"]["lit"]["type"] == "num") {
                    fwrite(STDERR, "ERR: Chybejici RelOp.\n");
                    exit(80);
                }
            }
        }

        function handleArgs()
        {
            global $argv;
            foreach ($argv as $arg) {
                if ($arg == "--help") {                                         // --help found
                    $this->checkFlags("hf");
                } elseif ($arg == "-h") {                                       // short -h found
                    $this->checkFlags("hf");
                } elseif ($arg == "-n") {                                       // -n found
                    $this->checkFlags("nf");
                } elseif (preg_match("/^--input=.+/", $arg)) {                  // --input=FILENAME found
                    $this->checkFlags("if");
                    $this->params["inputFile"] = trim(substr($arg, 8), "\n");
                } elseif (preg_match("/^-i=.+/", $arg)) {                       // short -i=FILENAME found
                    $this->checkFlags("if");
                    $this->params["inputFile"] = trim(substr($arg, 3), "\n");
                } elseif (preg_match("/^--output=.+/", $arg)) {                 // --output=FILENAME found
                    $this->checkFlags("of");
                    $this->params["outputFile"] = trim(substr($arg, 9), "\n");
                }elseif (preg_match("/^-o=.+/", $arg)) {                        // short -o=FILENAME found
                    $this->checkFlags("of");
                    $this->params["outputFile"] = trim(substr($arg, 3), "\n");
                } elseif (preg_match("/^--query=.+/", $arg)) {                  // --query='query' found
                    $this->checkFlags("qf");
                    $this->params["query"] = trim(substr($arg, 8), "\n");
                } elseif (preg_match("/^-q=.+/", $arg)) {                       // short -q='query' found
                    $this->checkFlags("qf");
                    $this->params["query"] = trim(substr($arg, 3), "\n");
                } elseif (preg_match("/^--qf=.+/", $arg)) {                     // --qf=FILENAME found
                    $this->checkFlags("qff");
                    $this->params["queryFile"] = trim(substr($arg, 5), "\n");
                } elseif (preg_match("/^--root=.+/", $arg)) {                   // --root=NAME found
                    $this->checkFlags("rf");
                    $this->params["rootElem"] = trim(substr($arg, 7), "\n");
                }elseif ( preg_match("/^-r=.+/", $arg)) {                       // short -r=NAME found
                    $this->checkFlags("rf");
                    $this->params["rootElem"] = trim(substr($arg, 3), "\n");
                } else {
                    if ($arg == $argv[0]) {
                        ;
                    } else {                                                    // invalid arg. found
                        fwrite(STDERR, "ERR: Zadan neplatny argument.\n");
                        exit(1);
                    }
                }
            }
        }

        // validate input and output file
        function checkFiles() {

            // check if inputFile exists and is readable
            if ($this->flags["if"] && (!file_exists($this->params["inputFile"]) || !is_readable($this->params["inputFile"]))) {
                fwrite(STDERR, "ERR: XML soubor neexistuje, nebo je necitelny.\n");
                exit(2);
            }

            // check if outputFile is writeable
            if (file_exists($this->params["outputFile"]) && !is_writable($this->params["outputFile"])) {
                fwrite(STDERR, "ERR: Do vystupniho XML souboru nelze zapisovat.\n");
                exit(3);
            }
        }

        //create SimpleXMLElement
        function parseXml() {
            if (!$this->flags["if"]) {
                // read file from STDIN
                while ($f[] = fgets(STDIN)) {;}
                $this->xml = @simpleXML_load_file(implode("",$f),"SimpleXMLElement");
            } else {
                // Create SimpleXMLElement from FILE
                $this->xml = @simpleXML_load_file($this->params["inputFile"],"SimpleXMLElement");
                if ($this->xml === FALSE) {
                    fwrite(STDERR, "ERR: XML soubor neni validni.\n");
                    exit(4);
                }

            }
        }

        // Loops through SimpleXMLElement recursively and find elements that matches
        function findEl($name, $src, $where = "undefined") {
            // name contains element.attribute
            if(is_array($name) && $name["el"] != "" && $name["at"] != "") {
                foreach ($src->children() as $child)
                {
                    if($child->getName() == $name["el"]){
                        foreach($child->attributes() as $a => $b) {
                            if($child->getName() == $name["el"] && $a == $name["at"]) {
                                $this->xmlDom[] = $child->asXML()."\n";         // el and .at matches
                            }
                            else {
                                $this->findEl($name, $child);                   // go deeper
                            }
                        }
                    }else {
                        $this->findEl($name, $child);
                    }
                }
                $this->xmlDom = array_slice($this->xmlDom, 0, 1);
            }else if(is_array($name) && $name["el"] == "" && $name["at"] != "" ) { // name contains only .attribute
                foreach ($src->children() as $child)
                {
                    $flag = true;
                    foreach($child->attributes() as $a => $b) {
                        if($a == $name["at"]) {
                            $flag = false;
                            $this->xmlDom[] = $child->asXML()."\n";             // .at matches
                        }
                        else {
                            $this->findEl($name, $child);
                        }
                    }
                    if($flag) $this->findEl($name, $child);
                }
                $this->xmlDom = array_slice($this->xmlDom, 0, 1);
            }else { // name contains only element
                if(is_array($name)) {
                    $name = $name["el"];
                }
                if($src->getName() == $name) {
                    $lmao = preg_replace('/<\?xml .*version=\".*>\n/', '', $src->asXML());
                    $this->xmlDom[] = $lmao;
                }
                foreach ($src->children() as $child)
                {
                    if($child->getName() == $name) {
                        $this->xmlDom[] = $child->asXML()."\n";
                    }else {
                        $this->findEl($name, $child);                           // go deeper
                    }
                }
            }
        }

        function whereFilter($add, $where, $c) {
            foreach ($c->children() as $value) {
                fwrite(STDERR, "ERR: Chybny format vstupniho souboru.\n");
                exit(4);
            }
            if($where["relOp"] == "CONTAINS"){
                if($where["lit"]["type"] != "str") {              // contains and numeric literal
                    fwrite(STDERR, "ERR: Chybny format vstupniho souboru.\n");
                    exit(4);
                }
                if(strpos($c, $where["lit"]["val"]) !== false && !$where["not"]) {
                    $this->xmlDom[] = $add->asXML()."\n";
                }else if(strpos($c, $where["lit"]["val"]) === false && $where["not"]) {
                    $this->xmlDom[] = $add->asXML()."\n";
                }
            }elseif ($where["relOp"] == ">") {
                if($where["lit"]["type"] == "num") {
                    if(floatval($c) > $where["lit"]["val"]) {
                        $this->xmlDom[] = $add->asXML()."\n";
                    }
                }else {
                    if(strcmp($c,$where["lit"]["val"]) > 0) {
                        $this->xmlDom[] = $add->asXML()."\n";
                    }
                }
            }elseif ($where["relOp"] == "<") {
                if($where["lit"]["type"] == "num") {
                    if(floatval($c) < $where["lit"]["val"]) {
                        $this->xmlDom[] = $add->asXML()."\n";
                    }
                }else {
                    if(strcmp($c,$where["lit"]["val"]) < 0) {
                        $this->xmlDom[] = $add->asXML()."\n";
                    }
                }
            }elseif ($where["relOp"] == "=") {
                if($where["lit"]["type"] == "num") {
                    if(floatval($c) == $where["lit"]["val"]) {
                        $this->xmlDom[] = $add->asXML()."\n";
                    }
                }else {
                    if(strcmp($c,$where["lit"]["val"]) == 0) {
                        $this->xmlDom[] = $add->asXML()."\n";
                    }
                }
            }
        }

        //
        function where($where, $src, $add="none") {
            $fl = false;        // flag needed for 1st iteration of where function
            $level = false;
            if($add == "none") $fl = true;
            foreach ($src->children() as $child) {
                if($fl == true) {
                    $add = $child;
                }

                // Element has .attribute
                if($where["el"]["at"] != "") {
                    foreach ($child->attributes() as $a => $b) {
                        // if element's name matches with WHERE el
                        if($where["el"]["el"] == $child->getName()) {
                            // and .attribute is found
                            if($where["el"]["at"] == $a ) {
                                $this->whereFilter($add, $where, $b);
                                $level = true;
                            }
                        // if element has no name, only .attribute
                        }else if($where["el"]["el"] == "") {
                            // and .attribute is found
                            if($where["el"]["at"] == $a )
                            {
                                $this->whereFilter($add, $where, $b);
                                $level = true;
                            }
                        }
                    }
                // Element has no attributes and it's name matches with WHERE el
                }else if($where["el"]["el"] == $child->getName() && $where["el"]["at"] == "") {
                    $this->whereFilter($add, $where, $child);
                    $level = true;
                }else {

                }
                // No acceptable results on this level. We need to go DEEPER!
                if(!$level) $this->where($where, $child, $add);
            }
        }

        // WHERE pre-function
        function whCall($rootEl, $src) {
            // Create SIMPLEXMLELEMENT
            $xmlStart = "<?xml version=\"1.0\" encoding=\"UTF-8\"?><".$rootEl.">\n";
            $this->xmlDom = array();
            $xmlEnd = "</".$rootEl.">\n";

            // choose between WHERE and WHERE NOT
            if($this->queryElems["where"]["not"]) {
                $w1 = $this->queryElems["where"];
                $w1["not"] = !$w1["not"];
                $this->where($w1,$src); // make complement
                $backup = $this->xmlDom;

                $this->xmlDom = array();
                foreach ($src->children() as $child) {
                    if (!in_array($child->asXML()."\n", $backup)) {
                        $this->xmlDom[] = $child->asXML()."\n";
                    }
                }
            }else {
                $this->where($this->queryElems["where"],$src);
            }
            return simplexml_load_string(implode("",array($xmlStart,implode("",$this->xmlDom),$xmlEnd)));
        }

        // SELECT and FIND pre-function
        function select($queryEl, $rootEl, $src, $from = "LMAO_no") {
            // this function is used with SELECT and FROM
            if(is_array($rootEl) && $rootEl["el"] == "" && $rootEl["at"] == "") {
                fwrite(STDERR, "ERR: Chybny format vstupniho souboru.\n");
                exit(80);
            }else {
                $xmlStart = "<?xml version=\"1.0\" encoding=\"UTF-8\"?><".$rootEl.">\n";
                $this->xmlDom = array();
                $xmlEnd = "</".$rootEl.">\n";

                // SELECT book FROM book
                if(is_array($this->queryElems["from"]) && $queryEl == $this->queryElems["from"]["el"] && $from=="kek") {
                    foreach ($src->children() as $kek) {
                        foreach ($kek->children() as $ke) {
                            $this->findEl($queryEl,$ke);
                        }
                    }
                }else {
                    $this->findEl($queryEl,$src);
                }
                // if FROM call, select only 1st element that matches
                if($from == "from"){
                    $this->xmlDom = array_slice($this->xmlDom, 0, 1);
                }

                return simplexml_load_string(implode("",array($xmlStart,implode("",$this->xmlDom),$xmlEnd)));
            }
        }

        // LIMIT function, cuts array of all elements
        function limitEl($limit, $src) {
            // Apply LIMIT
            foreach ($src->children() as $child)
            {
                $this->xmlDom[] = $child->asXML()."\n";
            }
            if ($limit != "unlimited") {
                $this->xmlDom = array_slice($this->xmlDom, 0,$limit);
            }
        }

        // LIMIT pre-function
        function limitCall($limit, $rootEl,$src) {
            // Create SIMPLEXMLELEMENT
            $xmlStart = "<?xml version=\"1.0\" encoding=\"UTF-8\"?><ROOT>\n";
            $this->xmlDom = array();
            $xmlEnd = "</ROOT>\n";
            $this->limitEl($limit,$src);

            return simplexml_load_string(implode("",array($xmlStart,implode("",$this->xmlDom),$xmlEnd)));
        }


        function orderAdd($order, $src, $add="none") {
            $fl = false;        // flag needed for 1st iteration of where function
            $level = false;
            if($add == "none") $fl = true;
            foreach ($src->children() as $child) {
                if($fl == true) {
                    $add = $child;
                }

                // Element has .attribute
                if($order["el"]["at"] != "") {
                    foreach ($child->attributes() as $a => $b) {
                        // if element's name matches with WHERE el
                        if($order["el"]["el"] == $child->getName()) {
                            // and .attribute is found
                            if($order["el"]["at"] == $a ) {
                                $this->xmlDom[] = $b->__toString();
                                $level = true;
                            }
                        // if element has no name, only .attribute
                        }else if($order["el"]["el"] == "") {
                            // and .attribute is found

                            if($order["el"]["at"] == $a )
                            {
                                $this->xmlDom[] = $b->__toString();
                                $level = true;
                            }
                        }
                    }
                // Element has no attributes and it's name matches with WHERE el
                }else if($order["el"]["el"] == $child->getName() && $order["el"]["at"] == "") {
                    $this->xmlDom[] = $child->__toString();
                    $level = true;
                }
                // No acceptable results on this level. We need to go DEEPER!
                if(!$level) $this->orderAdd($order, $child, $add);
            }

        }

        function orderExecute($order, &$orAddArr, $src, $add="none") {
            $fl = false;        // flag needed for 1st iteration of where function
            $level = false;
            if($add == "none") $fl = true;
            foreach ($src->children() as $child) {
                if($fl == true) {
                    $add = $child;
                }

                // Element has .attribute
                if($order["el"]["at"] != "") {
                    foreach ($child->attributes() as $a => $b) {
                        // if element's name matches with WHERE el
                        if($order["el"]["el"] == $child->getName()) {
                            // and .attribute is found
                            if(count($orAddArr) > 0 && $order["el"]["at"] == $a && $orAddArr[0] == $b) {
                                array_splice($orAddArr, 0, 1);
                                $add->addAttribute('order', count($this->xmlDom)+1);
                                $this->xmlDom[] = $add->asXML()."\n";
                                $level = true;
                            }
                        // if element has no name, only .attribute
                    }else if($order["el"]["el"] == "") {
                            // and .attribute is found

                            if(count($orAddArr) > 0 && $order["el"]["at"] == $a && $orAddArr[0] == $b)
                            {
                                array_splice($orAddArr, 0, 1);
                                $add->addAttribute('order', count($this->xmlDom)+1);
                                $this->xmlDom[] = $add->asXML()."\n";
                                $level = true;
                            }
                        }
                    }
                // Element has no attributes and it's name matches with WHERE el
            }else if(count($orAddArr) > 0 && $order["el"]["el"] == $child->getName() && $order["el"]["at"] == "" && $orAddArr[0] == $child) {
                    array_splice($orAddArr, 0, 1);
                    $add->addAttribute('order', count($this->xmlDom)+1);
                    $this->xmlDom[] = $add->asXML()."\n";
                    $level = true;
                }
                // No acceptable results on this level. We need to go DEEPER!
                if(!$level) $this->orderExecute($order, $orAddArr, $child, $add);
            }

        }

        // ORDER pre-function
        function orderCall($order, $src) {
            $xmlStart = "<?xml version=\"1.0\" encoding=\"UTF-8\"?><ROOT>\n";
            $this->xmlDom = array();
            $xmlEnd = "</ROOT>\n";
            // Find all elements to sort
            $this->orderAdd($order, $src);
            if(count($this->xmlDom) == 0) {
                fwrite(STDERR, "ERR: Chyba vstupniho souboru.\n");
                exit(4);
            }
            // Save array of values
            $orAddArr = $this->xmlDom;
            // Sort array of values
            natsort($orAddArr);
            if($order["way"] == "DESC") {
                $orAddArr = array_reverse($orAddArr, true);
            }
            // key values are not sorted
            foreach ($orAddArr as $key => $value) {
                $sortArr[] = $value;
            }

            // Clean xmlDom array
            $this->xmlDom = array();
            while(count($sortArr) > 0) {
                // Apply sort
                $this->orderExecute($order, $sortArr, $src);
            }

            return simplexml_load_string(implode("",array($xmlStart,implode("",$this->xmlDom),$xmlEnd)));
        }

        // MAIN FUNCTION of xqr
        function execute() {

            $this->handleArgs();
            if ($this->flags["hf"]) { $this->help();}
            if (!($this->flags["qf"] or $this->flags["qff"])) {
                fwrite(STDERR, "ERR: Chybi --qf i --query.\n");
                exit(4);
            }

            // check if input and output files are valid
            $this->checkFiles();

            // read XML query
            $this->getQuery();

            // validate query
            $this->validateQuery($this->query);

            // open and parse xml file
            $this->parseXml();
            $temp = $this->xml;

            // FROM filter
            if($this->queryElems["from"] != "ROOT") {
                $temp = $this->select($this->queryElems["from"],$this->queryElems["root"],$this->xml,"from");
            }
            // SELECT filter
            $temp = $this->select($this->queryElems["select"],$this->queryElems["root"],$temp, "kek");
            // WHERE filter if needed
            if($this->queryElems["whSet"]) {
                $temp = $this->whCall($this->queryElems["root"],$temp);
            }
            // ORDER BY filter if needed
            if(is_array($this->queryElems["orderBy"])) {
                $temp = $this->orderCall($this->queryElems["orderBy"],$temp);
            }
            // LIMIT filter if needed
            if($this->queryElems["limit"] != "unlimited") $temp = $this->limitCall($this->queryElems["limit"],$this->queryElems["root"],$temp);

            $finalXml = $temp->asXML();
            if($this->flags["nf"]) {
                $finalXml = preg_replace('/^.+\n/', '', $finalXml);
            }
            $fp = fopen($this->params["outputFile"], 'w');

            if($this->flags["rf"]) {
                // if there's --root param, rename <ROOT> element
                //------// if there's empty <ROOT> element, make it into <ROOT /> but properly renamed
                $finalXml = preg_replace('/<(.*)>[\n\t]*<\/\1>/', '<'.$this->params["rootElem"].' />', $finalXml);
                $finalXml = preg_replace('/<ROOT>/', '<'.$this->params["rootElem"].'>', $finalXml);
                $finalXml = preg_replace('/<\/ROOT>/', '</'.$this->params["rootElem"].'>', $finalXml);
            }else {
                // if there's no --root param. simply delete <ROOT> element
                $finalXml = preg_replace('/<ROOT>\n/', '', $finalXml);
                $finalXml = preg_replace('/<\/ROOT>\n/', '', $finalXml);
            }

            // write to output file
            fwrite($fp, $finalXml);


        }


}

$xqr = new Xqr;
$xqr->execute();

exit(0);
?>
