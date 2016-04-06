initial_doc.pdf: initial_doc.md graph.pdf legend.pdf metadata.yaml
	pandoc -s -o $@ initial_doc.md metadata.yaml

graph.pdf: graph.dot
	neato -T pdf $^ > $@

legend.pdf: legend.dot
	dot -T pdf $^ > $@
