namespace DijkstrasAlgorithm
{
    using System.Linq;

    public class Vertex
    {
        private readonly string _Name;

        public Vertex(string name)
        {
            _Name = name;
            Cost = int.MaxValue;
            Parent = null;
        }

        public Edge AdjacentEdge { get; private set; }

        public int Cost { get; set; }

        public string Name
        {
            get
            {
                return _Name;
            }
        }

        public Vertex Parent { get; set; }

        public bool IsAdjacentTo(Vertex v, Edge[] edges)
        {
            var connectingEdges = from e in edges
                                  where e.ConnectedTo(v)
                                  && e.ConnectedTo(this)
                                  orderby e.Cost
                                  select e;
            this.AdjacentEdge = connectingEdges.FirstOrDefault();
            return this.AdjacentEdge != null;
        }

        public override string ToString()
        {
            string p = Parent != null ? Parent.Name : "-";
            string c = Cost < int.MaxValue ? Cost.ToString() : "-";
            return string.Format("{0} ({1}, {2})", Name, p, c);
        }
    }
}