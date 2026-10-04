;; -*- lexical-binding: t; -*-
(setq inhibit-startup-screen t)
(tool-bar-mode 0)
(menu-bar-mode 0)
(scroll-bar-mode 0)
(ido-mode 1)
(setq ido-enable-flex-matching t)
(setq ido-everywhere t)
(global-display-line-numbers-mode 1)
(setq-default display-line-numbers-type 'relative)
(set-face-attribute 'default nil
                    :family "CodeNewRomanNerdFont"
                    :height 180
                    :width 'regular)
(setq c-ts-indent-offset 4)

(add-to-list 'load-path "~/.emacs.d/packages/color-theme-sanityinc-tomorrow")
(add-to-list 'custom-theme-load-path
             "~/.emacs.d/packages/color-theme-sanityinc-tomorrow")
(load-theme 'sanityinc-tomorrow-night t)

(add-to-list 'load-path "~/.emacs.d/packages/smex")
(require 'smex)
(smex-initialize)
(global-set-key (kbd "M-x") 'smex)
(global-set-key (kbd "M-X") 'smex-major-mode-commands)

(add-to-list 'load-path "~/.emacs.d/packages/posframe")
(add-to-list 'load-path "~/.emacs.d/packages/company-mode")
(require 'company-childframe)
(require 'company)
(add-hook 'after-init-hook 'global-company-mode)
(setq company-idle-delay nil)
(global-set-key (kbd "M-TAB") 'company-complete)

(add-to-list 'load-path "~/.emacs.d/packages/cond-let")
(add-to-list 'load-path "~/.emacs.d/packages/llama")
(add-to-list 'load-path "~/.emacs.d/packages/with-editor/lisp")
(add-to-list 'load-path "~/.emacs.d/packages/magit/lisp")
(require 'magit)

(add-to-list 'load-path "~/.emacs.d/packages/multiple-cursors.el")
(require 'multiple-cursors)
(global-set-key (kbd "C-S-c C-S-c") 'mc/edit-lines)
(global-set-key (kbd "C->") 'mc/mark-next-like-this)
(global-set-key (kbd "C-<") 'mc/mark-previous-like-this)
(global-set-key (kbd "C-c C-<") 'mc/mark-all-like-this)

(setq treesit-language-source-alist
      '((bash "https://github.com/tree-sitter/tree-sitter-bash")
	(c "https://github.com/tree-sitter/tree-sitter-c")
	(c++ "https://github.com/tree-sitter/tree-sitter-cpp")
	(cmake "https://github.com/uyha/tree-sitter-cmake")
	(css "https://github.com/tree-sitter/tree-sitter-css")
	(html "https://github.com/tree-sitter/tree-sitter-html")
	(json "https://github.com/tree-sitter/tree-sitter-json")
	(toml "https://github.com/tree-sitter/tree-sitter-toml")
	(yaml "https://github.com/ikatyang/tree-sitter-yaml")))

(setq major-mode-remap-alist
      '((yaml-mode . yaml-ts-mode)
	(html-mode . html-ts-mode)
	(sh-mode . bash-ts-mode)
	(c-mode . c-ts-mode)
	(c++-mode . c++-ts-mode)
	(json-mode . json-ts-mode)
	(cmake-mode . cmake-ts-mode)
	(css-mode . css-ts-mode)))

(customize-set-variable 'treesit-font-lock-level 4)

(with-eval-after-load 'eglot
  (add-to-list 'eglot-server-programs
               '((c-ts-mode c++-ts-mode) . ("clangd")))
  (add-to-list 'eglot-server-programs
               '(bash-ts-mode . ("bash-language-server")))
  (add-to-list 'eglot-server-programs
               '(cmake-ts-mode . ("cmake-language-server")))
  (add-to-list 'eglot-server-programs
               '(yaml-ts-mode . ("yaml-language-server")))
  (add-to-list 'eglot-server-programs
               '(json-ts-mode . ("vscode-langservers-extracted")))
  (add-to-list 'eglot-server-programs
               '(css-ts-mode . ("vscode-langservers-extracted")))
  (add-to-list 'eglot-server-programs
               '(html-ts-mode . ("vscode-langservers-extracted")))
  
  )

(add-hook 'c-ts-mode-hook 'eglot-ensure)
(add-hook 'c++-ts-mode-hook 'eglot-ensure)
(add-hook 'cmake-ts-mode-hook 'eglot-ensure)
(add-hook 'css-ts-mode-hook 'eglot-ensure)
(add-hook 'html-ts-mode-hook 'eglot-ensure)
(add-hook 'json-ts-mode-hook 'eglot-ensure)
(add-hook 'taml-ts-mode-hook 'eglot-ensure)

(add-hook 'eglot-managed-mode-hook
          (lambda ()
            (eglot-inlay-hints-mode 0)))

