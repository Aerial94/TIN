initial_doc.pdf: initial_doc.md graph.pdf
	pandoc -s -o $@ initial_doc.md

graph.pdf: graph.dot
	neato -T pdf $^ > $@

