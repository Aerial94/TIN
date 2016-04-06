initial_doc.pdf: initial_doc.md graph.pdf metadata.yaml
	pandoc -s -o $@ initial_doc.md metadata.yaml

graph.pdf: graph.dot
	neato -T pdf $^ > $@

