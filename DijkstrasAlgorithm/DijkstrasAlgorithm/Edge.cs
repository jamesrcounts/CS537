using System;
using System.Collections.Generic;
using System.Linq;

namespace DijkstrasAlgorithm
{
    public class Edge
    {
        private readonly int _Cost;
        private readonly Vertex[] ends;

        public Edge(int cost, Vertex v, Vertex u)
        {
            _Cost = cost;
            ends = new[] { v, u };
        }

        public int Cost
        {
            get
            {
                return _Cost;
            }
        }

        public Vertex[] Ends
        {
            get
            {
                return ends;
            }
        }

        public bool ConnectedTo(Vertex v)
        {
            return Ends.Any(n => n == v);
        }

        public Vertex OtherNode(Vertex that)
        {
            return Ends.Where(v => v != that).Single();
        }

        public override string ToString()
        {
            return string.Format(
                "{0} ({1},{2})",
                Cost,
                this.Ends[0].Name,
                this.Ends[1].Name);
        }
    }
}