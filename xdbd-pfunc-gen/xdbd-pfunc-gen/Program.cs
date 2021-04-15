using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;

namespace sql_gen
{
    class Program
    {
        static void Main(string[] args)
        {
            Program pgm = new Program();
            foreach (string grammar in args)
            {
                pgm.Generate(grammar);
            }
        }

        int Generate(string grammar)
        {
            uint kind = 0;
            GrammarClass currentClass = null;
            bool firstLine = true;
            StreamReader reader = new StreamReader(grammar);

            while (!reader.EndOfStream)
            {
                string  line = reader.ReadLine();
                string[] parts;
                string[] types;
                if (firstLine)
                {
                    kind = 1;
                    firstLine = false;
                    parts = line.Split(new char[] { ':' }, StringSplitOptions.RemoveEmptyEntries);
                    if (parts.Length != 2)
                        continue;
                    currentClass = new GrammarClass (parts[0]);
                    m_classes[parts[0]] = currentClass;
                    types = parts[1].Split(new char[] { ' ' }, StringSplitOptions.RemoveEmptyEntries);
                }
                else
                {
                    types = line.Split(new char[] { '|', ' ' }, StringSplitOptions.RemoveEmptyEntries);
                    if (types.Length == 0)
                    {
                        firstLine = true;
                        continue;
                    }
                }
                currentClass.AddClass(kind++, types);
                //              Console.WriteLine (line);
            }

            foreach (GrammarClass gclass in m_classes.Values)
            {
                foreach (TypeClass tclass in gclass.m_types)
                {
                    string[] types = tclass.types;
                    for (int i = 0; i < types.Length; ++i)
                    {
                        string name = types[i];
                        try
                        {
                            GrammarClass g = m_classes[name];
                        }
                        catch (Exception)
                        {
                            types[i] = "char";
                        }
                    }
                    tclass.GenerateNames();
                }
                for (int i = 0; i < gclass.m_types.Count; ++i)
                {
                    if (gclass.m_types[i].disabled)
                        continue;
                    for (int j = i + 1; j < gclass.m_types.Count; ++j)
                    {
                        if (gclass.m_types[j].Equals(gclass.m_types[i]))
                        {
                            gclass.m_types[i].incvariants();
                            gclass.m_types[j].disable();
                        }
                    }
                }
                gclass.GenerateNames();
            }

            StreamWriter writer = new StreamWriter("SqlSyntax.h");
            writer.WriteLine ("#pragma once");
            writer.WriteLine ();
            writer.WriteLine ("#include \"StdAfx.h\"");
            writer.WriteLine ();

            writer.WriteLine("typedef enum _SqlSyntaxContext");
            writer.WriteLine("{");
            writer.WriteLine("\tSqlFirstSyntaxContext,");
            foreach (GrammarClass gclass in m_classes.Values)
            {
                writer.WriteLine("\t" + gclass.name + "_Context,");
            }
            writer.WriteLine("\tSqlLastSyntaxContext");
            writer.WriteLine("} SqlSyntaxContext;");
            writer.WriteLine();

            writer.WriteLine("class\tSqlSyntax");
            writer.WriteLine ("{");
            writer.WriteLine ("public:");
            writer.WriteLine ("\tSqlSyntax (unsigned int kind, SqlSyntaxContext context);");
            writer.WriteLine ("\tvirtual\t~SqlSyntax ();");
            writer.WriteLine ("\tinline unsigned int kind () { return m_kind; }");
            writer.WriteLine ("\tinline void* getAny () { return m_any; }");
            writer.WriteLine ("\tinline void setAny (void* any) { m_any = any; }");
            writer.WriteLine ("\tinline SqlSyntaxContext context () { return m_context; }");
            writer.WriteLine ("\tinline SqlSyntax* parent () { return m_parent; }");
            writer.WriteLine ("\tinline void parent (SqlSyntax* parent) { m_parent = parent; }");
            writer.WriteLine ("protected:");
            writer.WriteLine("\tunsigned int m_kind;");
            writer.WriteLine("\tSqlSyntaxContext m_context;");
            writer.WriteLine("\tSqlSyntax* m_parent;");
            writer.WriteLine("\tvoid* m_any;");
            writer.WriteLine ("};");
            writer.WriteLine ();

            foreach (GrammarClass gclass in m_classes.Values)
            {
                writer.WriteLine("class\t" + gclass.name + ";");
            }

            writer.Close();

            writer = new StreamWriter("SqlSyntax.cpp");
            writer.WriteLine ("#include \"SqlSyntax.h\"");
            writer.WriteLine ("");
            writer.WriteLine("SqlSyntax::SqlSyntax (unsigned int kind, SqlSyntaxContext context)");
            writer.WriteLine ("{");
            writer.WriteLine("\tm_kind = kind;");
            writer.WriteLine("\tm_context = context;");
            writer.WriteLine("\tm_parent = 0;");
            writer.WriteLine("\tm_any = 0;");
            writer.WriteLine ("}");
            writer.WriteLine ("");
            writer.WriteLine ("SqlSyntax::~SqlSyntax () {}");
            writer.Close();

            StreamWriter cwriter = new StreamWriter("SqlCompilerCallbackDecls.h");
            foreach (GrammarClass gclass in m_classes.Values)
            {
                cwriter.WriteLine("\tvirtual\tvoid\tInvoke_" + gclass.name + "_Callback (SqlCallbackReason reason, unsigned int kind, " + gclass.name + "* c_" + gclass.name + ");");
            }
            cwriter.Close();

            cwriter = new StreamWriter("SqlCompilerCallbackImpls.cpp");
            cwriter.WriteLine();
            foreach (GrammarClass gclass in m_classes.Values)
            {
                cwriter.WriteLine("#if !defined(" + gclass.name + "_CallbackDefined)");
                cwriter.WriteLine("void\tCLASSNAME::Invoke_" + gclass.name + "_Callback (SqlCallbackReason reason, unsigned int kind, " + gclass.name + "* p_" + gclass.name + ")");
                cwriter.WriteLine("{");
                cwriter.WriteLine("\tswitch (reason)");
                cwriter.WriteLine("\t{");
                cwriter.WriteLine("\tcase\tDefaultCallbackReason:");
                cwriter.WriteLine("\t\tbreak;");
                cwriter.WriteLine("\tcase\tTraversalPrologueCallbackReason:");
                cwriter.WriteLine("\t\tbreak;");
                cwriter.WriteLine("\tcase\tTraversalEpilogueCallbackReason:");
                cwriter.WriteLine("\t\tbreak;");
                cwriter.WriteLine("\t}");
                cwriter.WriteLine("}");
                cwriter.WriteLine("#endif");
                cwriter.WriteLine();
            }
            cwriter.Close();

            cwriter = new StreamWriter("SqlCompilerCallbacks.h");
            cwriter.WriteLine("#pragma once");
            cwriter.WriteLine();
            cwriter.WriteLine("#include \"parse-syntax.h\"");
            cwriter.WriteLine("#include <stack>");
            cwriter.WriteLine("using namespace std;");
            cwriter.WriteLine();
            cwriter.WriteLine("enum\tSqlCallbackReason");
            cwriter.WriteLine("{");
            cwriter.WriteLine("\tDefaultCallbackReason = 0,");
            cwriter.WriteLine("\tTraversalPrologueCallbackReason = 1,");
            cwriter.WriteLine("\tTraversalEpilogueCallbackReason = 2");
            cwriter.WriteLine("};");
            cwriter.WriteLine();
            cwriter.WriteLine("class SqlCompilerCallbacks");
            cwriter.WriteLine("{");
            cwriter.WriteLine("\ttypedef\tstack < SqlSyntax* >\tpstack;");
            cwriter.WriteLine("public:");
            cwriter.WriteLine("\tSqlCompilerCallbacks () {}");
            cwriter.WriteLine("\tvirtual\t~SqlCompilerCallbacks () {}");
            cwriter.WriteLine();
            foreach (GrammarClass gclass in m_classes.Values)
            {
                cwriter.WriteLine("\tvirtual\tvoid\tInvoke_" + gclass.name + "_Callback (SqlCallbackReason reason, unsigned int kind, " + gclass.name + "* c_" + gclass.name + ") = 0;");
            }
            cwriter.WriteLine();
            cwriter.WriteLine("\tinline void push (SqlSyntax* p_syntax) { m_stack.push (p_syntax); }");
            cwriter.WriteLine("\tinline void pop () { m_stack.pop(); }");
            cwriter.WriteLine("\tinline SqlSyntax* top () { return m_stack.top(); }");
            cwriter.WriteLine("private:");
            cwriter.WriteLine("\tpstack m_stack;");
            cwriter.WriteLine("};");
            cwriter.Close();

            StreamWriter pwriter = new StreamWriter("parse-functions.h");
            pwriter.WriteLine ("#pragma once");
            pwriter.WriteLine ();
            foreach (GrammarClass gclass in m_classes.Values)
            {
                writer = new StreamWriter(gclass.name + ".h");
                writer.WriteLine("#pragma once");
                writer.WriteLine ();
                writer.WriteLine("#include \"SqlSyntax.h\"");
                List<string> includes = new List<string>();
                foreach (ParameterClass par in gclass.parameters.Values)
                {
                    if (par.ptype.CompareTo("char") == 0)
                        continue;
                    if (includes.Contains(par.ptype))
                        continue;
                    includes.Add(par.ptype);
                }
                foreach (string incl in includes)
                {
                    writer.WriteLine("#include \"" + incl + ".h\"");
                }
                writer.WriteLine();
                writer.WriteLine("class\t" + gclass.name + " : public SqlSyntax");
                writer.WriteLine("{");
                writer.WriteLine("public:");
                foreach (TypeClass tclass in gclass.m_types)
                {
                    if (tclass.disabled)
                        continue;
                    string separator = "";
                    writer.Write("\t" + gclass.name + " (");
                    if (tclass.variants > 1)
                    {
                        writer.Write("unsigned int kind");
                        separator = ", ";
                    }
                    string[] types = tclass.types;
                    string[] names = tclass.names;
                    for (int i = 0; i < types.Length; ++i)
                    {
                        writer.Write(separator + types[i] + "* c_" + names[i]);
                        separator = ", ";
                    }
                    writer.WriteLine(");");
                }
                writer.WriteLine("\tvirtual ~" + gclass.name + "();");
                writer.WriteLine("\tvoid Traverse (void* parseCtx);");
                writer.WriteLine("private:");
                writer.WriteLine("\tvoid _init ();");
                writer.WriteLine("");
                writer.WriteLine("public:");
                foreach (ParameterClass par in gclass.parameters.Values)
                {
                    if (par.ptype.CompareTo ("char") != 0)
                    {
                        writer.WriteLine ("\tinline\t" + par.ptype + "*\tget_" + par.pname + " () { return m_" + par.pname + "; }");
                        writer.WriteLine ("\tinline\tvoid\tset_" + par.pname + " (" + par.ptype + "* " + par.pname + ") { m_" + par.pname + " = " + par.pname + "; }");
                    }
                    else
                    {
                        writer.WriteLine ("\tinline\tstring\tget_" + par.pname + " () { return m_" + par.pname + "; }");
                        writer.WriteLine ("\tinline\tvoid\tset_" + par.pname + " (char* str) { m_" + par.pname + " = str; }");
                    }
                    if (par.ptype == gclass.name)
                        writer.WriteLine ("\t" + par.ptype + "*\trevert_" + par.ptype + " (" + par.ptype + "* p_" + par.ptype + ", u_int kind);");
                }
                kind = 0;
                writer.WriteLine("");
                writer.WriteLine("public:");
                foreach (TypeClass tclass in gclass.m_types)
                {
                    if (tclass.disabled)
                        continue;
                    for (int i = 0; i < tclass.variants; ++i)
                    {
                        ++kind;
                        writer.WriteLine("\tstatic const u_int\tg_" + gclass.name + "_" + kind + " = " + kind + ";");
                    }
                }
                writer.WriteLine("");
                writer.WriteLine("private:");
                foreach (ParameterClass par in gclass.parameters.Values)
                {
                    if (par.ptype.CompareTo("char") != 0)
                        writer.WriteLine("\t" + par.ptype + "*\tm_" + par.pname + ";");
                    else
                        writer.WriteLine("\tstring\tm_" + par.pname + ";");
                }
                writer.WriteLine("");
                writer.WriteLine("};");
                writer.Close();
                writer = new StreamWriter(gclass.name + ".cpp");
                writer.WriteLine("#include \"SqlCompilerCallbacks.h\"");
                writer.WriteLine("#include \"" + gclass.name + ".h\"");
                writer.WriteLine();
                foreach (TypeClass tclass in gclass.m_types)
                {
                    if (tclass.disabled)
                        continue;
                    string separator = "";
                    writer.WriteLine();
                    writer.Write(gclass.name + "::" + gclass.name + " (");
                    if (tclass.variants > 1)
                    {
                        writer.Write("unsigned int kind");
                        separator = ", ";
                    }
                    string[] types = tclass.types;
                    string[] names = tclass.names;
                    for (int i = 0; i < types.Length; ++i)
                    {
                        writer.Write(separator + types[i] + "* c_" + names[i]);
                        separator = ", ";
                    }
                    writer.Write(") : SqlSyntax (");
                    if (tclass.variants > 1)
                        writer.Write("kind");
                    else
                        writer.Write(tclass.kind);
                    writer.Write(", " + gclass.name + "_Context");
                    writer.WriteLine(")");
                    writer.WriteLine("{");
                    writer.WriteLine("\t_init ();");
                    for (int i = 0; i < types.Length; ++i)
                    {
                        writer.WriteLine("\tm_" + names[i] + " = " + "c_" + names[i] + ";");
                    }
                    writer.WriteLine("}");
                }
                writer.WriteLine();
                writer.WriteLine(gclass.name + "::~" + gclass.name + " ()");
                writer.WriteLine("{");
                foreach (ParameterClass par in gclass.parameters.Values)
                {
                    if (par.ptype.CompareTo("char") != 0)
                        writer.WriteLine("\tdelete\tm_" + par.pname + ";");
                }
                writer.WriteLine("\t_init ();");
                writer.WriteLine("}");
                writer.WriteLine();
                writer.WriteLine("void\t" + gclass.name + "::_init ()");
                writer.WriteLine("{");
                foreach (ParameterClass par in gclass.parameters.Values)
                {
                    if (par.ptype.CompareTo("char") != 0)
                        writer.WriteLine("\tm_" + par.pname + " = 0;");
                }
                writer.WriteLine("}");
                writer.WriteLine();
                writer.WriteLine("void " + gclass.name + "::Traverse (void* parseCtx)");
                writer.WriteLine("{");
                writer.WriteLine("\tunsigned int kind = this->kind();");
                writer.WriteLine("\t((SqlCompilerCallbacks*)parseCtx)->Invoke_" + gclass.name + "_Callback (TraversalPrologueCallbackReason, kind, this);");
                writer.WriteLine("\t((SqlCompilerCallbacks*)parseCtx)->push(this);");
                writer.WriteLine("\tswitch (kind)");
                writer.WriteLine("\t{");
                kind = 0;
                foreach (TypeClass tclass in gclass.m_types)
                {
                    if (tclass.disabled)
                        continue;
                    for (int i = 0; i < tclass.variants; ++i)
                    {
                        ++kind;
                        writer.WriteLine("\t\tcase\tg_" + gclass.name + "_" + kind + ":");

                        string[] names = tclass.names;
                        for (int j = 0; j < names.Length; ++j)
                        {
                            if (!names[j].StartsWith ("char"))
                                writer.WriteLine("\t\t\tm_" + names[j] + "->Traverse (parseCtx);");
                        }

                        writer.WriteLine ("\t\tbreak;");
                    }
                }
                writer.WriteLine("\t}");
                writer.WriteLine("\t((SqlCompilerCallbacks*)parseCtx)->pop();");
                writer.WriteLine("\t((SqlCompilerCallbacks*)parseCtx)->Invoke_" + gclass.name + "_Callback (TraversalEpilogueCallbackReason, kind, this);");
                writer.WriteLine("}");
                foreach (ParameterClass par in gclass.parameters.Values)
                {
                    if (par.pname != gclass.name)
                        continue;
                    writer.WriteLine();
                    writer.WriteLine(par.pname + "*\t" + par.pname + "::revert_" + par.pname + " (" + par.pname + "* p_" + par.pname + ", u_int kind)");
                    writer.WriteLine("{");
                    writer.WriteLine("\tif (this == 0)");
                    writer.WriteLine("\t\treturn\tp_" + par.pname + ";");
                    writer.WriteLine("\t" + par.pname + "*\tq_" + par.pname + " = m_" + par.pname + "->revert_" + par.pname + "(this, m_kind);");
                    writer.WriteLine("\tm_kind = kind;");
                    writer.WriteLine("\tm_" + par.pname + " = p_" + par.pname + ";");
                    writer.WriteLine("\treturn\t" + "q_" + par.pname + ";");
                    writer.WriteLine("}");
                }
                writer.WriteLine();
                writer.WriteLine("extern \"C\"");
                writer.WriteLine("{");
                kind = 0;
                foreach (TypeClass tclass in gclass.m_types)
                {
                    if (tclass.disabled)
                        continue;
                    for (int i = 0; i < tclass.variants; ++i)
                    {
                        ++kind;
                        writer.WriteLine();
                        writer.Write("void*\t" + gclass.name + "_" + kind + " (void* parseCtx");
                        string[] types = tclass.types;
                        string[] names = tclass.names;
                        for (int j = 0; j < types.Length; ++j)
                            writer.Write(", void* c_" + names[j]);
                        writer.WriteLine(")");
                        writer.WriteLine("{");
                        writer.Write("\t" + gclass.name + "*\tp_" + gclass.name + " = new " + gclass.name + " (");
                        string separator = "";
                        if (tclass.variants > 1)
                        {
                            writer.Write(kind);
                            separator = ", ";
                        }
                        for (int j = 0; j < types.Length; ++j)
                        {
                            writer.Write(separator + "(" + types[j] + "*)" + "c_" + names[j]);
                            separator = ", ";
                        }
                        writer.WriteLine(");");
                        for (int j = 0; j < types.Length; ++j)
                        {
                            if (types[j] == "char")
                                continue;
                            writer.WriteLine("\t((" + types[j] + "*)" + "c_" + names[j] + ")->parent (p_" + gclass.name + ");");
                        }
                        writer.WriteLine("\t((SqlCompilerCallbacks*) parseCtx)->Invoke_" + gclass.name + "_Callback (DefaultCallbackReason, " + kind + ", p_" + gclass.name + ");");
                        writer.WriteLine("\treturn\tp_" + gclass.name + ";");
                        writer.WriteLine("}");
                        pwriter.Write("void*\t" + gclass.name + "_" + kind + " (void* parseCtx");
                        for (int j = 0; j < types.Length; ++j)
                            pwriter.Write(", void* c_" + names[j]);
                        pwriter.WriteLine(");");
                    }
                }

                writer.WriteLine();
                writer.WriteLine("}");
                writer.Close();
            }
            pwriter.Close();
            pwriter = new StreamWriter("parse-syntax.h");
            pwriter.WriteLine ("#pragma once");
            pwriter.WriteLine ();
            pwriter.WriteLine("#if defined (__cplusplus)");
            pwriter.WriteLine("extern \"C\"");
            pwriter.WriteLine("{");
            pwriter.WriteLine("#endif");
            pwriter.WriteLine();
            foreach (GrammarClass gclass in m_classes.Values)
            {
                pwriter.WriteLine("#include\t\"" + gclass.name + ".h\"");
            }
            pwriter.WriteLine();
            pwriter.WriteLine("#if defined (__cplusplus)");
            pwriter.WriteLine("}");
            pwriter.WriteLine("#endif");
            pwriter.Close();
            return 0;
        }
        Dictionary<string, GrammarClass> m_classes = new Dictionary<string,GrammarClass> ();
    }

    class TypeClass
    {
        public TypeClass(uint kind, string[] types)
        {
            this.disabled = false;
            this.variants = 1;
            this.kind = kind;
            string[]    newtypes = new string[types.Length];
            int    index = 0;
            foreach (string t in types)
            {
                string    newt = "";
                string    separator = "";
                string[]    splits = t.Split(new char[] { '.' }, StringSplitOptions.RemoveEmptyEntries);
                foreach (string s in splits)
                {
                    newt += separator + s;
                    separator = "_";
                }
                newtypes [index++] = newt;
            }
            this.types = newtypes;
        }

        public bool Equals (TypeClass tclass)
        {
            string[] types = tclass.types;
            if (types.Length != this.types.Length)
                return false;
            for (int i = 0; i < types.Length; ++i)
                if (types[i].CompareTo(this.types[i]) != 0)
                    return false;
            return true;
        }

        public void incvariants()
        {
            ++variants;
        }

        public void disable()
        {
            disabled = true;
        }

        public void GenerateNames()
        {
            names = new string[types.Length];
            for (int i = 0; i < types.Length; ++i)
                names[i] = "";
            for (int i = 0; i < types.Length; ++i)
            {
                if (names[i] != "")
                    continue;
                int count = 1;
                for (int j = i + 1; j < types.Length; ++j)
                {
                    if (types[j].CompareTo(types[i]) == 0)
                    {
                        ++count;
                        names[j] = types[j] + '_' + count;
                    }
                }
                if (count > 1)
                    names[i] = types[i] + "_1";
                else
                    names[i] = types[i];
            }
        }

        public bool disabled { get; private set; }
        public uint variants { get; private set; }
        public uint kind { get; private set; }
        public string[] types { get; private set; }
        public string[] names { get; private set; }
    }

    class GrammarClass
    {
        public GrammarClass(string name)
        {
            string    newname = "";
            string    separator = "";
            string[] names = name.Split(new char[] { '.' }, StringSplitOptions.RemoveEmptyEntries);
            foreach (string n in names)
            {
                newname += separator + n;
                separator = "_";
            }
            this.name = newname;
        }

        public void AddClass(uint kind, string[] types)
        {
            m_types.Add(new TypeClass(kind, types));
        }

        public void GenerateNames()
        {
            foreach (TypeClass tclass in m_types)
            {
                string[] types = tclass.types;
                string[] names = tclass.names;
                for (int i = 0; i < names.Length; ++i)
                {
                    try
                    {
                        ParameterClass par = parameters[names[i]];
                    }
                    catch (Exception)
                    {
                        parameters[names[i]] = new ParameterClass(types[i], names[i]);
                    }
                }
            }
        }

        public string name { get; private set; }
        public List<TypeClass> m_types = new List<TypeClass>();
        public Dictionary<string, ParameterClass> parameters = new Dictionary<string, ParameterClass>();
    }

    class ParameterClass
    {
        public ParameterClass(string ptype, string pname)
        {
            this.ptype = ptype;
            this.pname = pname;
        }

        public string ptype { get; private set; }
        public string pname { get; private set; }
    }
}
