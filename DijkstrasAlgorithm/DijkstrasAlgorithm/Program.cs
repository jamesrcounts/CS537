using System;
using System.Collections.Generic;
using System.Linq;

namespace DijkstrasAlgorithm
{
    internal class Program
    {
        private static IEnumerable<Vertex> Dijkstra(
            Vertex[] nodes,
            Edge[] edges,
            Vertex source)
        {
            source.Cost = 0;

            var treeNodes = new List<Vertex>();
            var fringe = new List<Vertex>() { source };

            for (int i = 0; i < nodes.Length; i++)
            {
                var remainder = nodes.Where(n => !treeNodes.Contains(n)).ToList();
                var u = fringe.OrderBy(n => n.Cost).First();
                if (u != source)
                {
                    WritePathTo(u);
                }
                treeNodes.Add(u);
                foreach (var r in remainder)
                {
                    if (r.IsAdjacentTo(u, edges))
                    {
                        int c = u.Cost + r.AdjacentEdge.Cost;
                        if (c < r.Cost)
                        {
                            r.Cost = c;
                            r.Parent = u;
                        }

                        if (!fringe.Contains(r))
                        {
                            fringe.Add(r);
                        }
                    }
                }
                fringe.Remove(u);
                Console.Write("{0}|{1}|{2}", u, string.Join(", ", fringe), null);
            }

            return treeNodes;
        }

        private static string GetPath(Vertex v)
        {
            string result = string.Empty;
            for (Vertex current = v; current != null; current = current.Parent)
            {
                if (current.Parent != null)
                {
                    result = string.Format(" - {0}{1}", current.Name, result);
                }
                else
                {
                    result = current.Name + result;
                }
            }

            return result;
        }

        private static void Main()
        {
            Vertex[] nodes = new Vertex[]
            {
                new Vertex("a"),
                new Vertex("b"),
                new Vertex("c"),
                new Vertex("d"),
                new Vertex("e"),
                new Vertex("f"),
                new Vertex("g"),
                new Vertex("h"),
                new Vertex("i"),
                new Vertex("j"),
                new Vertex("k"),
                new Vertex("l"),
            };
            Edge[] edges = new[]
            {
                new Edge(3, nodes[0], nodes[1]),
                new Edge(5, nodes[0], nodes[2]),
                new Edge(4, nodes[0], nodes[3]),
                new Edge(3, nodes[1], nodes[4]),
                new Edge(6, nodes[1], nodes[5]),
                new Edge(2, nodes[2], nodes[3]),
                new Edge(1, nodes[3], nodes[4]),
                new Edge(2, nodes[4], nodes[5]),
                new Edge(4, nodes[2], nodes[6]),
                new Edge(5, nodes[3], nodes[7]),
                new Edge(4, nodes[4], nodes[8]),
                new Edge(5, nodes[5], nodes[9]),
                new Edge(3, nodes[6], nodes[7]),
                new Edge(6, nodes[7], nodes[8]),
                new Edge(3, nodes[8], nodes[9]),
                new Edge(6, nodes[6], nodes[10]),
                new Edge(7, nodes[7], nodes[10]),
                new Edge(5, nodes[8], nodes[11]),
                new Edge(9, nodes[9], nodes[11]),
                new Edge(8, nodes[10], nodes[11]),
            };
            Dijkstra(nodes, edges, nodes[0]);
        }

        private static void WritePathTo(Vertex v)
        {
            Console.WriteLine(
                "to {0}: {1} of length {2}",
                v.Name,
                GetPath(v),
                v.Cost);
        }
    }
}